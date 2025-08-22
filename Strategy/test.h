#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#include <vector>
#include <limits>

#include"Judge.h"
#include"Point.h"

#include <chrono>

const double C = 1.4;


double getTime();

class GameState {
public:
    GameState(const std::vector<std::vector<int>>& board, Point last_point, int who, int M, int N);

    bool is_win() const;
    bool is_draw() const;
    bool is_terminal() const;
    std::vector<Point> get_legal_actions() const;
    GameState get_next_state(Point point) const;
    int get_current_player() const;
    int get_last_player() const;
    int get_winner() const;
    Point get_last_action() const;


    std::vector<std::vector<int>> board;
    Point last_point;
    int last_user_or_machine;
    int M, N;
};
int** vectorTo2DArray(const std::vector<std::vector<int>>& vec);
void delete2DArray(int** board, int M);

class Node {
public:
    Node(GameState state);
    bool is_leaf();
    bool is_terminal();

    GameState state;
    Node* parent;
    std::vector<Node*> children;
    int visits;
    int wins;

    bool is_win;
    bool is_draw;

private:
    bool calculate_is_win() const;
    bool calculate_is_draw() const;
};

class MonteCarloTreeSearch {
public:
    MonteCarloTreeSearch(GameState state);
    ~MonteCarloTreeSearch();

    void deleteNode(Node* node, double startTime);

    Node* selection(Node* node);
    void expansion(Node* node);
    int simulation(Node* node);
    void backpropagation(Node* node, int winner);
    Point get_best_action(int simulations = 100);

private:
    Node* root;
};

