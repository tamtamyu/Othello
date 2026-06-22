import json
import random
from copy import deepcopy

SIZE = 8
EMPTY, BLACK, WHITE = 0, 1, 2

DIRECTIONS = [
    (-1, -1), (-1, 0), (-1, 1),
    (0, -1),          (0, 1),
    (1, -1),  (1, 0), (1, 1),
]

BASE_POSITION_SCORE = [
    [100, -20, 10,  5,  5, 10, -20, 100],
    [-20, -50, -2, -2, -2, -2, -50, -20],
    [ 10,  -2, -1, -1, -1, -1,  -2,  10],
    [  5,  -2, -1, -1, -1, -1,  -2,   5],
    [  5,  -2, -1, -1, -1, -1,  -2,   5],
    [ 10,  -2, -1, -1, -1, -1,  -2,  10],
    [-20, -50, -2, -2, -2, -2, -50, -20],
    [100, -20, 10,  5,  5, 10, -20, 100],
]


def opponent(player):
    return WHITE if player == BLACK else BLACK


class Othello:
    def __init__(self):
        self.board = [[EMPTY for _ in range(SIZE)] for _ in range(SIZE)]
        self.turn = BLACK
        self.reset()

    def reset(self):
        self.board = [[EMPTY for _ in range(SIZE)] for _ in range(SIZE)]
        self.board[3][3] = WHITE
        self.board[3][4] = BLACK
        self.board[4][3] = BLACK
        self.board[4][4] = WHITE
        self.turn = BLACK

    def inside(self, row, col):
        return 0 <= row < SIZE and 0 <= col < SIZE

    def can_place(self, row, col, player=None):
        if player is None:
            player = self.turn

        if not self.inside(row, col):
            return False

        if self.board[row][col] != EMPTY:
            return False

        enemy = opponent(player)

        for dr, dc in DIRECTIONS:
            r = row + dr
            c = col + dc
            found_enemy = False

            while self.inside(r, c) and self.board[r][c] == enemy:
                found_enemy = True
                r += dr
                c += dc

            if found_enemy and self.inside(r, c) and self.board[r][c] == player:
                return True

        return False

    def valid_moves(self, player=None):
        if player is None:
            player = self.turn

        moves = []

        for row in range(SIZE):
            for col in range(SIZE):
                if self.can_place(row, col, player):
                    moves.append((row, col))

        return moves

    def place(self, row, col):
        if not self.can_place(row, col, self.turn):
            return False

        player = self.turn
        enemy = opponent(player)

        self.board[row][col] = player

        for dr, dc in DIRECTIONS:
            r = row + dr
            c = col + dc
            flip_list = []

            while self.inside(r, c) and self.board[r][c] == enemy:
                flip_list.append((r, c))
                r += dr
                c += dc

            if self.inside(r, c) and self.board[r][c] == player:
                for fr, fc in flip_list:
                    self.board[fr][fc] = player

        self.turn = opponent(self.turn)
        return True

    def check_pass_or_finish(self):
        if self.valid_moves(self.turn):
            return "playing"

        other = opponent(self.turn)

        if self.valid_moves(other):
            self.turn = other
            return "passed"

        return "finished"

    def count(self, player):
        return sum(cell == player for row in self.board for cell in row)

    def winner(self):
        black = self.count(BLACK)
        white = self.count(WHITE)

        if black > white:
            return BLACK
        if white > black:
            return WHITE
        return EMPTY


def default_params():
    return {
        "position_score": deepcopy(BASE_POSITION_SCORE),
        "flip_weight": 1,
        "give_corner_penalty": -200,
        "mobility_weight": 3,
    }


def is_corner(row, col):
    return (row == 0 or row == SIZE - 1) and (col == 0 or col == SIZE - 1)


def evaluate_move(game, row, col, params):
    copy_game = deepcopy(game)

    player = copy_game.turn
    before = copy_game.count(player)

    copy_game.place(row, col)

    after = copy_game.count(player)
    flipped_count = after - before

    score = 0
    score += params["position_score"][row][col]
    score += flipped_count * params["flip_weight"]

    # 相手に角を取らせる手を減点
    for r, c in copy_game.valid_moves(copy_game.turn):
        if is_corner(r, c):
            score += params["give_corner_penalty"]

    # 自分の置ける手を増やし、相手の置ける手を減らす
    opponent_moves = len(copy_game.valid_moves(copy_game.turn))
    own_moves = len(copy_game.valid_moves(player))
    score += (own_moves - opponent_moves) * params["mobility_weight"]

    return score


def choose_move(game, params):
    moves = game.valid_moves(game.turn)

    if not moves:
        return None

    best_score = -10**9
    best_moves = []

    for row, col in moves:
        score = evaluate_move(game, row, col, params)

        if score > best_score:
            best_score = score
            best_moves = [(row, col)]
        elif score == best_score:
            best_moves.append((row, col))

    return random.choice(best_moves)


def play_game(black_params, white_params):
    game = Othello()

    while True:
        params = black_params if game.turn == BLACK else white_params
        move = choose_move(game, params)

        if move is not None:
            game.place(move[0], move[1])

        state = game.check_pass_or_finish()

        if state == "finished":
            return game.winner()


def mutate_params(params):
    new_params = deepcopy(params)

    # 位置評価を少し変える
    for row in range(SIZE):
        for col in range(SIZE):
            if random.random() < 0.2:
                new_params["position_score"][row][col] += random.randint(-5, 5)

    # 各重みを少し変える
    new_params["flip_weight"] += random.choice([-1, 0, 1])
    new_params["mobility_weight"] += random.choice([-1, 0, 1])
    new_params["give_corner_penalty"] += random.randint(-20, 20)

    new_params["flip_weight"] = max(0, new_params["flip_weight"])
    new_params["mobility_weight"] = max(0, new_params["mobility_weight"])
    new_params["give_corner_penalty"] = min(-50, new_params["give_corner_penalty"])

    return new_params


def battle(params_a, params_b, games=50):
    score_a = 0
    score_b = 0

    for i in range(games):
        if i % 2 == 0:
            winner = play_game(params_a, params_b)

            if winner == BLACK:
                score_a += 1
            elif winner == WHITE:
                score_b += 1
        else:
            winner = play_game(params_b, params_a)

            if winner == WHITE:
                score_a += 1
            elif winner == BLACK:
                score_b += 1

    return score_a, score_b


def train(generations=200, games_per_generation=50):
    best_params = default_params()

    for gen in range(1, generations + 1):
        challenger = mutate_params(best_params)

        best_score, challenger_score = battle(
            best_params,
            challenger,
            games=games_per_generation
        )

        if challenger_score > best_score:
            best_params = challenger
            result = "採用"
        else:
            result = "維持"

        print(
            f"世代 {gen:03d}: "
            f"現AI {best_score}勝 / 新AI {challenger_score}勝 → {result}"
        )

    return best_params


def save_params(params, filename="ai_params.json"):
    with open(filename, "w", encoding="utf-8") as f:
        json.dump(params, f, ensure_ascii=False, indent=4)


if __name__ == "__main__":
    random.seed()

    params = train(
        generations=200,
        games_per_generation=50
    )

    save_params(params)

    print("学習完了: ai_params.json を保存しました")