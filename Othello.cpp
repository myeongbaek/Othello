#include <bangtal>
#include <iostream>
using namespace bangtal;
using namespace std;

ObjectPtr board[8][8];
ObjectPtr score[2][17];

int score_value = 2, score_value_b = 2, score_value_w = 2, color = 0;
int nb = 2, nw = 2;
int game_start = 0;

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;

void setState(int x, int y, State s) {
	switch (s) {
	case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
	case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
	case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	board_state[y][x] = s;
}

bool checkPossible(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;
		}
		else { return false; }
	}

	return false;
}

bool checkPossible(int x, int y) {

	if (board_state[y][x] == State::BLACK) return false;
	if (board_state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);

	int delta[8][2] = {
		{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
	};
/*
	for (int i = 0; i < 8; i++) {
		checkPossible(x, y, delta[i][0], delta[i][1]);
	}
*/
	bool possible = false;
	for (auto d : delta) {
		if(checkPossible(x, y, d[0], d[1])) possible = true;
	}

	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);
						turn == Turn::BLACK ? (nb += 1, nw -= 1): (nb -= 1, nw += 1);
					}
					else return;
				}
			}
		}
		else return;
	}
}

void reverse(int x, int y) {

	int delta[8][2] = {
		{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
	};
	/*
		for (int i = 0; i < 8; i++) {
			checkPossible(x, y, delta[i][0], delta[i][1]);
		}
	*/
	bool possible = false;
	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);
	}
}

bool setPossible() {

	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

void ChangeSore() {
	
	score[0][score_value_b % 10]->hide();
	if (score_value_b >= 10) score[0][score_value_b / 10 + 10]->hide();
	score[1][score_value_w % 10]->hide();
	if (score_value_w >= 10) score[1][score_value_w / 10 + 10]->hide();
	

	score[0][nb % 10]->show();
	if (nb >= 10) score[0][nb / 10 + 10]->show();
	score[1][nw % 10]->show();
	if (nw >= 10) score[1][nw / 10 + 10]->show();

	score_value_b = nb, score_value_w = nw;

	
}

void computer() {
	if (setPossible()) {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (board_state[y][x] == State::POSSIBLE) {
					setState(x, y, State::WHITE);
					nw += 1;
					reverse(x, y);
					ChangeSore();
					turn = Turn::BLACK;
					if (!setPossible()) {
						turn = Turn::WHITE;
						computer();
					}
					return;
				}
			}
		}
	}
	else {
		turn = Turn::BLACK;
		printf("%d", setPossible());
		if (!setPossible()) {
			if (nb > nw) showMessage("Black wins!!");
			else showMessage("White wins!!");
		}
		return;
	}
}


int main() {

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	auto scene = Scene::create("Othello", "Images/background.png");

	string str = "Images/L0.png";
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 17; j++) {
			if (!i) {
				str.replace(8, 1, to_string(j % 10));
				score[i][j] = Object::create(str, scene, 820, 230, false);
				if (j > 10) {
					score[i][j]->locate(scene, 780, 230);
				}
			}
			else {
				str.replace(8, 1, to_string(j % 10));
				score[i][j] = Object::create(str, scene, 1120, 230, false);
				if (j > 10) {
					score[i][j]->locate(scene, 1080, 230);
				}
			}
		}
	}
	
	auto timer = Timer::create(1.0f);
	timer->setOnTimerCallback([&](TimerPtr)->bool { computer(); timer->set(1.0f); return true; });

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80, true);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction)->bool {
				if (board_state[y][x] == State::POSSIBLE) {
					if (turn == Turn::BLACK) {
						setState(x, y, State::BLACK);
						nb += 1;
						reverse(x, y);
						ChangeSore();
					
						turn = Turn::WHITE;
						timer->start();
					}
					
					
				}
				return true;
			});
			board_state[y][x] = State::BLANK;
		}
	}
	
	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);
	setPossible();
	score[0][2]->show();
	score[1][2]->show();

	startGame(scene);
}