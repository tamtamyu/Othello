import json
import random
import argparse
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

        return [
            (row, col)
            for row in range(SIZE)
            for col in range(SIZE)
            if self.can_place(row, col, player)
        ]

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
        "mobility_weight": 3,
        "give_corner_penalty": -200,
    }


def load_params(path):
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def save_params(params, path):
    with open(path, "w", encoding="utf-8") as f:
        json.dump(params, f, ensure_ascii=False, indent=4)


def is_corner(row, col):
    return (row == 0 or row == SIZE - 1) and (col == 0 or col == SIZE - 1)


def evaluate_move(game, row, col, params):
    copy_game = deepcopy(game)

    player = copy_game.turn
    enemy = opponent(player)

    before = copy_game.count(player)
    copy_game.place(row, col)
    after = copy_game.count(player)

    flipped_count = after - before

    score = 0

    score += params["position_score"][row][col]
    score += flipped_count * params["flip_weight"]

    for r, c in copy_game.valid_moves(enemy):
        if is_corner(r, c):
            score += params["give_corner_penalty"]

    own_moves = len(copy_game.valid_moves(player))
    enemy_moves = len(copy_game.valid_moves(enemy))

    score += (own_moves - enemy_moves) * params["mobility_weight"]

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
            return game.winner(), game.count(BLACK), game.count(WHITE)


def battle(params_a, params_b, games=100):
    a_wins = 0
    b_wins = 0
    draws = 0

    for i in range(games):
        if i % 2 == 0:
            winner, _, _ = play_game(params_a, params_b)

            if winner == BLACK:
                a_wins += 1
            elif winner == WHITE:
                b_wins += 1
            else:
                draws += 1

        else:
            winner, _, _ = play_game(params_b, params_a)

            if winner == BLACK:
                b_wins += 1
            elif winner == WHITE:
                a_wins += 1
            else:
                draws += 1

    return a_wins, b_wins, draws


def mutate_params(params, mutation_rate=0.2):
    new_params = deepcopy(params)

    for row in range(SIZE):
        for col in range(SIZE):
            if random.random() < mutation_rate:
                new_params["position_score"][row][col] += random.randint(-5, 5)

    if random.random() < 0.5:
        new_params["flip_weight"] += random.choice([-1, 0, 1])

    if random.random() < 0.5:
        new_params["mobility_weight"] += random.choice([-1, 0, 1])

    if random.random() < 0.5:
        new_params["give_corner_penalty"] += random.randint(-20, 20)

    new_params["flip_weight"] = max(0, new_params["flip_weight"])
    new_params["mobility_weight"] = max(0, new_params["mobility_weight"])
    new_params["give_corner_penalty"] = min(-50, new_params["give_corner_penalty"])

    return new_params


def train(input_path=None, generations=200, games_per_generation=50, mutation_rate=0.2):
    if input_path:
        best_params = load_params(input_path)
        print(f"開始AI: {input_path}")
    else:
        best_params = default_params()
        print("開始AI: default_params")

    for gen in range(1, generations + 1):
        challenger = mutate_params(best_params, mutation_rate)

        best_wins, challenger_wins, draws = battle(
            best_params,
            challenger,
            games=games_per_generation
        )

        if challenger_wins > best_wins:
            best_params = challenger
            result = "採用"
        else:
            result = "維持"

        print(
            f"世代 {gen:04d}: "
            f"現AI {best_wins}勝 / 新AI {challenger_wins}勝 / 引分 {draws} → {result}"
        )

    return best_params


def run_battle(path_a, path_b, games):
    params_a = load_params(path_a)
    params_b = load_params(path_b)

    a_wins, b_wins, draws = battle(params_a, params_b, games)

    total = a_wins + b_wins + draws

    print("=== AI対戦結果 ===")
    print(f"AI A: {path_a}")
    print(f"AI B: {path_b}")
    print(f"対局数: {total}")
    print(f"AI A 勝利: {a_wins}勝 ({a_wins / total * 100:.1f}%)")
    print(f"AI B 勝利: {b_wins}勝 ({b_wins / total * 100:.1f}%)")
    print(f"引き分け: {draws}局 ({draws / total * 100:.1f}%)")


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--mode", choices=["train", "battle"], default="train")

    parser.add_argument("--input", default=None)
    parser.add_argument("--output", default="ai_params.json")

    parser.add_argument("--ai-a", default="ai_params.json")
    parser.add_argument("--ai-b", default="ai_params.json")

    parser.add_argument("--generations", type=int, default=200)
    parser.add_argument("--games", type=int, default=50)
    parser.add_argument("--mutation-rate", type=float, default=0.2)

    args = parser.parse_args()

    if args.mode == "train":
        params = train(
            input_path=args.input,
            generations=args.generations,
            games_per_generation=args.games,
            mutation_rate=args.mutation_rate
        )

        save_params(params, args.output)
        print(f"学習完了: {args.output} を保存しました")

    elif args.mode == "battle":
        run_battle(args.ai_a, args.ai_b, args.games)


if __name__ == "__main__":
    random.seed()
    main()