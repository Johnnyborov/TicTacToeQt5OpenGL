#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <QObject>
#include <vector>

enum class SquareTypes : unsigned char { Clear = 0, Cross, Nought };

enum class WinTypes: unsigned char { Crosses, Noughts, Draw };
enum class Directions: unsigned char { Right, Down, RightDown, LeftDown, None };

struct Conditions {
  WinTypes win_type;
  Directions direction;
  int start_i;
  int start_j;
};

class GameEngine: public QObject {
  Q_OBJECT

public:
  GameEngine();

public slots:
  void newGame();
  void makeMove(int i);

signals:
  void gameCreated(int dim_x, int dim_y, int win_size);
  void moveMade(int i, SquareTypes type);
  void gameOver(Conditions conditions);

private:
  void checkGame();
  WinTypes calculateWinner();

  int m_dim_x = 3;
  int m_dim_y = 3;
  int m_win_size = 3;

  std::vector<SquareTypes> m_squares;

  int m_count = 0;
  bool m_over = false;
  Conditions m_over_conditions;
};

#endif // GAME_ENGINE_H
