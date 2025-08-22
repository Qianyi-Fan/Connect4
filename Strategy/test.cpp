
#include"test.h"
// 只考虑当前这一步棋

// const int M =6; 
// const int N =7; 
// const int *top; const int *_board;
// const int lastX; const int lastY; const int noX; const int noY;








Node::Node(GameState state) : state(state), parent(nullptr), visits(0), wins(0) {
    is_win = calculate_is_win();
    is_draw = calculate_is_draw();
}

bool Node::is_leaf() {
    return children.empty();
}

bool Node::is_terminal() {
    // In this example, the game ends when a player wins or it's a draw
    return state.is_win() || state.is_draw();
}

bool Node::calculate_is_win() const {
    return state.is_win();
}

bool Node::calculate_is_draw() const {
    return state.is_draw();
}


MonteCarloTreeSearch::MonteCarloTreeSearch(GameState state) : root(new Node(state)) {}

MonteCarloTreeSearch::~MonteCarloTreeSearch() {
    
    double startTime = getTime();
    deleteNode(root, startTime);
}

void MonteCarloTreeSearch::deleteNode(Node* node, double startTime) {


    if (node == nullptr) {
        return;
    }

    for (Node* child : node->children) {
        deleteNode(child,startTime);
    }

    delete node;

    double currentTime = getTime();
    if (currentTime - startTime >= 0.5) return;

}


Node* MonteCarloTreeSearch::selection(Node* node) {
    
    double max_ucb = -std::numeric_limits<double>::infinity();
    Node* selected_node = nullptr;
    for (Node* child : node->children) {
        double ucb;
        if (child->visits == 0) {
            ucb = std::numeric_limits<double>::infinity();
        } else {
            ucb = static_cast<double>(child->wins) / child->visits +
                     C * std::sqrt(std::log(node->visits) / child->visits);
        }
        if (ucb > max_ucb) {
            max_ucb = ucb;
            selected_node = child;
        }
    }
    return selected_node;
}

void MonteCarloTreeSearch::expansion(Node* node) {
    std::vector<Point> possible_points = node->state.get_legal_actions();
    for (Point point : possible_points) {
        GameState next_state = node->state.get_next_state(point);
        Node* new_node = new Node(next_state);
        new_node->parent = node;
        node->children.push_back(new_node);
    }
}

int MonteCarloTreeSearch::simulation(Node* node) {
    GameState current_state = node->state;
    while (!current_state.is_terminal()) {
        std::vector<Point> possible_points = current_state.get_legal_actions();
        Point executive_point = possible_points[rand() % possible_points.size()];

        if (current_state.last_user_or_machine == 1){
            current_state.board[executive_point.x][executive_point.y] = 2;
            current_state.last_user_or_machine = 2;
        }
        else if(current_state.last_user_or_machine == 2){
            current_state.board[executive_point.x][executive_point.y] = 1;
            current_state.last_user_or_machine = 1;
        }
        current_state.last_point = executive_point;
    }
    return current_state.get_winner();
}

void MonteCarloTreeSearch::backpropagation(Node* node, int winner) {
    while (node != nullptr) {
        node->visits++;
        if (node->state.get_current_player() == winner) {
            node->wins++;
        }
        node = node->parent;
    }
}

Point MonteCarloTreeSearch::get_best_action(int simulations) {
    double s_time = getTime();

    int i = 0;
    for (; i < simulations; ++i) {

        if (getTime() - s_time > 2) break;

        Node* current_node = root;
        while (!current_node->is_leaf()) {
            current_node = selection(current_node);
        }

        if (current_node->visits == 0) {
            expansion(current_node);
            
            if (!current_node->children.empty())  current_node = current_node->children[0];
        }
        int winner = simulation(current_node);
        backpropagation(current_node, winner);

    }




    // double max_ucb = -std::numeric_limits<double>::infinity();
    double max_win_prob = -std::numeric_limits<double>::infinity();

    Node* best_node = nullptr;

    for (Node* child : root->children) {
        // double ucb;
        // if (child->visits == 0) {
        //     ucb = std::numeric_limits<double>::infinity();
        // }
        // else {
        //     ucb = static_cast<double>(child->wins) / child->visits +
        //                 C * std::sqrt(std::log(root->visits) / child->visits);
        // }

        double win_prob = static_cast<double>(child->wins) / child->visits;

        if (win_prob > max_win_prob) {
            max_win_prob = win_prob;
            best_node = child;
        }
    }

    return best_node->state.get_last_action();
}

// Example GameState class

GameState::GameState(const std::vector<std::vector<int>>& board, Point last_point, int who, int M, int N) 
    : board(board), last_point(last_point), last_user_or_machine(who), M(M), N(N){}

bool GameState::is_win() const {
    int** converted_board = vectorTo2DArray(board);

    if (last_point.x < 0 || last_point.y < 0) return false;

    bool win = userWin(last_point.x, last_point.y, M, N, converted_board) || 
            machineWin(last_point.x, last_point.y, M, N, converted_board);
    delete2DArray(converted_board, M);
    return win;
}

bool GameState::is_draw() const {
    int** converted_board = vectorTo2DArray(board);

    if (last_point.x < 0 || last_point.y < 0) return false;

    bool win = userWin(last_point.x, last_point.y, M, N, converted_board) || 
            machineWin(last_point.x, last_point.y, M, N, converted_board);
    delete2DArray(converted_board, M);

    if (!win) {
        for (int i = 0; i < N; i++) {
            if (board[0][i] == 0) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool GameState::is_terminal() const {
    return is_win() || is_draw();
}

std::vector<Point> GameState::get_legal_actions() const {

    if (is_terminal()) return {};

    std::vector<Point> legal_actions;

    for (int col = 0; col < N; ++col) {
        for (int row = M - 1; row >= 0; --row) {
            if (board[row][col] == 0) {
                legal_actions.push_back({row, col});
                break;
            }
        }
    }

    return legal_actions;
}

GameState GameState::get_next_state(Point point) const {
    std::vector<std::vector<int>> new_board = board;

    if (last_user_or_machine == 1) {
        new_board[point.x][point.y] = 2;
    } else {
        new_board[point.x][point.y] = 1;
    }

    return GameState(new_board, point, (last_user_or_machine == 1) ? 2 : 1, M, N);
}

int GameState::get_current_player() const {
    return last_user_or_machine;
}

int GameState::get_last_player() const {
    return last_user_or_machine;
}

int GameState::get_winner() const {
    return last_user_or_machine;
}

Point GameState::get_last_action() const {
    return last_point;
}


// 将 std::vector<std::vector<int>> 转换为 int *const *
int** vectorTo2DArray(const std::vector<std::vector<int>>& vec) {
    int M = vec.size();
    int N = vec[0].size();
    int** new_board = new int*[M];
    for (int i = 0; i < M; ++i) {
        new_board[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            new_board[i][j] = vec[i][j];
        }
    }
    return new_board;
}

// 释放动态分配的内存
void delete2DArray(int** board, int M) {
    for (int i = 0; i < M; ++i) {
        delete[] board[i];
    }
    delete[] board;
}



double getTime() {
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}

// int main() {
//     // 初始化一个 6x7 的棋盘
//     const int M = 6; // 行数
//     const int N = 7; // 列数
//     std::vector<std::vector<int>> board = {
//         {0, 0, 0, 0, 0, 0, 0},
//         {0, 0, 0, 0, 0, 0, 0},
//         {0, 0, 0, 0, 0, 0, 0},
//         {0, 0, 0, 0, 0, 0, 0},
//         {1, 0, 0, 1, 2, 2, 1},
//         {1, 0, 0, 1, 2, 2, 2}
//     };

//     // 使用初始化的 board 创建 initial_state
//     GameState initial_state(board, {4, 0}, 1); // 假设最后一步是用户下的棋，当前轮到机器下棋

//     // 创建 MCTS 对象并获取最佳动作
//     MonteCarloTreeSearch mcts(initial_state);
//     Point best_action = mcts.get_best_action(10000);

//     // 输出最佳动作的坐标
//     std::cout << "Best action: " << best_action.x << " " << best_action.y << std::endl;

//     return 0;
// }