#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//* to do
////アイテム情報を記憶
////playerのパラメータを記憶
////敵のパラメータを記憶
//*バトルフェーズの記述
//*ゴールの設定

#define LINE 7
#define COLUMN 20
#define P_LOCATION_X 1
#define P_LOCATION_Y 1
#define E_LOCATION_X 6
#define E_LOCATION_Y 2
#define MAX_TRY 1000
#define MAX_ITEM_COUNT 5
#define MAX_ENEMY_COUNT 7

typedef struct
{
    char name[50];
    int power;
    int hp;
    int max_hp;
    int defense;
    int type; // 0:使用 1: 武器 2: 盾 3: 装備
} Item_effect;

typedef struct
{
    char name[32];
    int level;
    int exp;
    int power;
    int hp;
    int max_hp;
    int defense;
} Character;

typedef struct
{
    Character base;
    Item_effect inventory[15];
    int level_up;
} Player;

typedef struct
{
    char mark;
    int is_wall;
    int is_player;
    int is_enemy;
    int is_item;
    Item_effect item;
    Character enemy;
} Cell;

void stage_init(Cell stage[][LINE]);
void print_stage(Cell stage[][LINE]);
void game_play(Cell stage[][LINE], int *player_x, int *player_y, int *gaming, Player *player, int *item_num, Item_effect items[], int *item_count, int *enemy_count, Character enemies[]);
int battle(Cell stage[][LINE], int *player_x, int *player_y, Player *player, Item_effect items[], int *item_num);
Item_effect item_pick(Item_effect item[]);
Character enemy_pick(Character enemies[]);
void battle_printer(Cell stage[][LINE], int *player_x, int *player_y, Player *player);
int positive_sub(int a, int b)
{
    if (a > b)
        return a - b;
    else
        return 0;
}
int positive_sum(int a, int b)
{
    if (a + b < 0)
        return 0;
    else
        return (a + b);
}
int chance_n_percent(int N)
{
    if (N >= 9)
    {
        N = 9;
    }
    int n = (10 - N) * 10;
    if (rand() % 100 <= (n - 1))
        return 1;
    else
        return 0;
}

int main(int argument_count, char *arguments[])
{ // gameload new_world.dat（すでにあるゲームをロード）
    Item_effect items[20] = {
        {"回復の薬（小）", 0, 30, 0, 0, 0},
        {"回復の薬（中）", 0, 70, 0, 0, 0},
        {"回復の薬（大）", 0, 120, 0, 0, 0},
        {"勇者の剣", 80, 0, 0, 0, 1},
        {"木の盾", 0, 0, 10, 20, 2},
        {"鉄の盾", 0, 0, 20, 60, 2},
        {"鋼鉄の鎧", -20, 0, 40, 80, 3},
        {"プロテイン", 40, 0, 0, 0, 0},
        {"ハンバーガー", 20, 10, 10, 10, 0},
        {"水", 0, 20, 20, 20, 0},
        {"成績表", 15, -15, -15, 60, 0},
        {"黒板", 150, -5, 0, -80, 1},
        {"ウソハチ", 300, -30, -50, -100, 1},
        {"自己防衛の心得", 0, 0, 40, 200, 0},
        {"蟹光線", 100, -20, 0, 0, 0},
    };
    Character enemies[70] = {
        {"スライム", 1, 10, 30, 40, 40, 20},
        {"スライム", 4, 20, 60, 100, 100, 80},
        {"スライム", 7, 60, 150, 300, 300, 200},
        {"ゴブリン", 2, 18, 45, 45, 45, 40},
        {"ゴブリン", 5, 50, 100, 180, 180, 90},
        {"ゴブリン", 8, 70, 280, 500, 500, 250},
        {"オーク", 3, 19, 90, 60, 60, 60},
        {"オーク", 6, 65, 250, 200, 200, 150},
        {"オーク", 9, 150, 400, 800, 800, 350},
        {"バット", 1, 8, 25, 30, 30, 15},
        {"バット", 4, 22, 75, 100, 100, 70},
        {"バット", 7, 65, 200, 400, 400, 180},
        {"ゴースト", 2, 15, 35, 40, 40, 30},
        {"ゴースト", 5, 55, 130, 160, 160, 100},
        {"ゴースト", 8, 70, 270, 900, 900, 250},
        {"ゴースト", 10, 200, 550, 950, 950, 500},
        {"スライム", 1, 10, 25, 30, 30, 10},
        {"スライム", 2, 18, 40, 45, 45, 30},
        {"スライム", 4, 20, 60, 100, 100, 80},
        {"スライム", 6, 60, 95, 180, 180, 90},
        {"ファイアスピリット", 5, 65, 280, 100, 100, 50},
        {"ファイアスピリット", 6, 70, 300, 160, 160, 80},
        {"ファイアスピリット", 8, 90, 350, 250, 250, 100},
        {"ファイアスピリット", 10, 270, 500, 600, 600, 200},
        {"ケイブラット", 3, 19, 90, 40, 40, 40},
        {"ケイブラット", 5, 55, 150, 120, 120, 120},
        {"ケイブラット", 7, 68, 280, 320, 320, 200},
        {"ケイブラット", 9, 150, 400, 600, 600, 300},
        {"スケルトン", 3, 18, 80, 45, 45, 60},
        {"スケルトン", 6, 65, 160, 180, 180, 150},
        {"スケルトン", 9, 200, 280, 550, 550, 280},
        {"オオカミ", 1, 12, 40, 30, 30, 30},
        {"オオカミ", 4, 25, 95, 90, 90, 90},
        {"オオカミ", 7, 80, 250, 800, 800, 250},
        {"マタンゴ", 2, 18, 50, 40, 40, 50},
        {"マタンゴ", 5, 60, 140, 190, 190, 180},
        {"マタンゴ", 8, 130, 290, 700, 700, 260},
        {"ゴブリンチーフ", 11, 280, 570, 980, 980, 580}};
    Player player;
    int item_num = 0, item_count = 0;
    int enemy_count = 0;
    if (argument_count == 2 && strcmp(arguments[0], "./gameworld") == 0)
    {
        FILE *fp = fopen(arguments[1], "r");
        if (fp == NULL)
        {
            printf("ファイルを開くことができませんでした。: %s\n", arguments[1]);
            return 1;
        }
        Cell stage[COLUMN][LINE];
        int player_x = 0, player_y = 0, gaming = 1;
        char line[1024]; // 一時的な保存先
        if (fgets(line, sizeof(line), fp) == NULL)
        {
            printf("ファイル形式が不正です。\n");
            fclose(fp);
            return 1;
        }
        item_num = atoi(strtok(line, ",\n"));
        for (int i = 0; i < LINE; i++)
        {
            if (fgets(line, sizeof(line), fp) == NULL)
                break; // 1行読み込み
            // lineを,\nで分割
            char *cell = strtok(line, ",\n");
            for (int j = 0; j < COLUMN && cell != NULL; j++)
            {
                stage[j][i].mark = cell[0];
                stage[j][i].is_wall = (cell[0] == '#');
                if (cell[0] == 'P')
                {
                    stage[j][i].is_player = 1;
                    stage[j][i].is_enemy = 0;
                    stage[j][i].is_item = 0;
                    player_x = j;
                    player_y = i;
                    player.base.level = atoi(strtok(NULL, ",\n"));
                    player.base.exp = atoi(strtok(NULL, ",\n"));
                    player.base.power = atoi(strtok(NULL, ",\n"));
                    player.base.hp = atoi(strtok(NULL, ",\n"));
                    player.base.max_hp = atoi(strtok(NULL, ",\n"));
                    player.base.defense = atoi(strtok(NULL, ",\n"));
                    for (int temp = 0; temp < item_num; temp++)
                    {
                        cell = strtok(NULL, ",\n");
                        strcpy(player.inventory[temp].name, cell);
                        player.inventory[temp].power = atoi(strtok(NULL, ",\n"));
                        player.inventory[temp].hp = atoi(strtok(NULL, ",\n"));
                        player.inventory[temp].max_hp = atoi(strtok(NULL, ",\n"));
                        player.inventory[temp].defense = atoi(strtok(NULL, ",\n"));
                        player.inventory[temp].type = atoi(strtok(NULL, ",\n"));
                    }
                }

                else if (cell[0] == 'E')
                {
                    stage[j][i].is_enemy = 1;
                    stage[j][i].is_player = 0;
                    stage[j][i].is_item = 0;
                    stage[j][i].enemy.level = atoi(strtok(NULL, ",\n"));
                    stage[j][i].enemy.exp = atoi(strtok(NULL, ",\n"));
                    stage[j][i].enemy.power = atoi(strtok(NULL, ",\n"));
                    stage[j][i].enemy.hp = atoi(strtok(NULL, ",\n"));
                    stage[j][i].enemy.max_hp = atoi(strtok(NULL, ",\n"));
                    stage[j][i].enemy.defense = atoi(strtok(NULL, ",\n"));
                }

                else if (cell[0] == 'I')
                {
                    stage[j][i].is_item = 1;
                    stage[j][i].is_enemy = 0;
                    stage[j][i].is_player = 0;
                    cell = strtok(NULL, ",\n");
                    strcpy(stage[j][i].item.name, cell);
                    stage[j][i].item.power = atoi(strtok(NULL, ",\n"));
                    stage[j][i].item.hp = atoi(strtok(NULL, ",\n"));
                    stage[j][i].item.max_hp = atoi(strtok(NULL, ",\n"));
                    stage[j][i].item.defense = atoi(strtok(NULL, ",\n"));
                    stage[j][i].item.type = atoi(strtok(NULL, ",\n"));
                    item_count++;
                }
                cell = strtok(NULL, ",\n");
            }
        }
        fclose(fp);
        printf("=== GAME WORLD LOADED ===\n");
        printf("World size: %d×%d\n", LINE, COLUMN);
        printf("Player location: (%d, %d)\n", player_x, player_y);
        while (gaming)
        {
            game_play(stage, &player_x, &player_y, &gaming, &player, &item_num, items, &item_count, &enemy_count, enemies);
        }

        print_stage(stage);
        return 0;
    }

    if (argument_count == 1 && strcmp(arguments[0], "./gameworld") == 0)
    {
        int i, j;
        Cell stage[COLUMN][LINE];
        for (i = 0; i < LINE; i++) // ステージの初期化
        {
            for (j = 0; j < COLUMN; j++)
            {
                stage[j][i].mark = '.';
                stage[j][i].is_wall = 0;
                stage[j][i].is_player = 0;
                stage[j][i].is_enemy = 0;
                stage[j][i].is_item = 0;
            }
        }
        printf("プレイヤーの名前を入力してください（50文字以内）: ");
        while (1)
        {
            if (fgets(player.base.name, sizeof(player.base.name), stdin) == NULL)
            {
                printf("問題が発生しました。\n");
                return 1;
            }
            // 改行が含まれているかチェック
            if (strchr(player.base.name, '\n') == NULL)
            {
                // 改行がない＝長すぎる入力
                printf("名前が長すぎます。50文字以内で再入力してください: ");
                // 標準入力の残りをクリア
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
                continue;
            }
            // 改行除去
            player.base.name[strcspn(player.base.name, "\n")] = '\0';
            break;
        }
        player.base.name[strcspn(player.base.name, "\n")] = '\0'; // 改行除去
        stage_init(stage);
        stage[P_LOCATION_X][P_LOCATION_Y].mark = 'P'; // プレイヤーの位置設定
        stage[P_LOCATION_X][P_LOCATION_Y].is_player = 1;
        stage[E_LOCATION_X][E_LOCATION_Y].mark = 'E'; // 敵の位置設定
        stage[E_LOCATION_X][E_LOCATION_Y].is_enemy = 1;
        stage[E_LOCATION_X][E_LOCATION_Y].enemy.level = 4; // 敵の情報
        stage[E_LOCATION_X][E_LOCATION_Y].enemy.exp = 20;
        stage[E_LOCATION_X][E_LOCATION_Y].enemy.power = 60;
        stage[E_LOCATION_X][E_LOCATION_Y].enemy.hp = 100;
        stage[E_LOCATION_X][E_LOCATION_Y].enemy.max_hp = 100;
        stage[E_LOCATION_X][E_LOCATION_Y].enemy.defense = 80;
        strcpy(stage[E_LOCATION_X][E_LOCATION_Y].enemy.name, "スライム"); // 敵のタイプ
        enemy_count++;

        player.base.level = 1; // プレイヤーの情報
        player.base.exp = 0;
        player.base.power = 10;
        player.base.hp = 60;
        player.base.max_hp = 60;
        player.base.defense = 0;
        player.level_up = 2;

        Item_effect item;
        srand((unsigned int)time(NULL)); // ランダム配置
        int r_y = rand() % (LINE - 2) + 1;
        int r_x = rand() % (COLUMN - 2) + 1;
        if (!(r_x == 1 && r_y == 1))
        {
            stage[r_x][r_y].mark = 'I';
            stage[r_x][r_y].is_item = 1; // アイテムの位置設定
        }
        item = item_pick(items);
        stage[r_x][r_y].item = item; // アイテム情報
        item_count++;
        int player_x = 1, player_y = 1, gaming = 1; // gamingは一回の行動の最後にインクリメント
        printf("=== NEW GAME STARTED ===\n");
        printf("World size: %d×%d\n", LINE, COLUMN);
        printf("Player location: (%d, %d)\n", player_x, player_y);

        while (gaming)
        {
            game_play(stage, &player_x, &player_y, &gaming, &player, &item_num, items, &item_count, &enemy_count, enemies);
        }
        print_stage(stage);
    }
    return 0;
}

void stage_init(Cell stage[][LINE]) // ステージ初期化
{
    int i, j;
    for (i = 0; i < LINE; i++)
    {
        for (j = 0; j < COLUMN; j++)
        {
            stage[j][i].mark = '.';
            stage[j][i].is_wall = 0;
        }
    }
    for (i = 0; i < COLUMN; i++)
    { // 上下壁
        stage[i][0].mark = '#';
        stage[i][0].is_wall = 1;
        stage[i][6].mark = '#';
        stage[i][6].is_wall = 1;
    }
    for (i = 0; i < LINE; i++)
    { // 左右壁
        stage[0][i].mark = '#';
        stage[0][i].is_wall = 1;
        stage[19][i].mark = '#';
        stage[19][i].is_wall = 1;
    }
}

void print_stage(Cell stage[][LINE])
{
    int i, j;
    for (i = 0; i < LINE; i++)
    {
        for (j = 0; j < COLUMN; j++)
        {
            printf("%c", stage[j][i].mark);
        }
        printf("\n");
    }
}

Item_effect item_pick(Item_effect item[])
{
    int r = rand() % 15;
    return item[r];
}
Character enemy_pick(Character enemies[])
{
    int r = rand() % 38;
    return enemies[r];
}

void game_play(Cell stage[][LINE], int *player_x, int *player_y, int *gaming, Player *player, int *item_num, Item_effect items[], int *item_count, int *enemy_count, Character enemies[])
{
    char input[50];
    int move = 0;
    int try_count = 0; // ランダム生成でループし過ぎを防止する上限カウント用

    if ((*enemy_count) < MAX_ENEMY_COUNT && *gaming % 6 == 4)
    {
        Character enemy;
        int r_x, r_y;
        do
        {
            r_y = rand() % (LINE - 2) + 1;
            r_x = rand() % (COLUMN - 2) + 1;
            if (try_count > MAX_TRY)
            {
                break;
            }
        } while (stage[r_x][r_y].is_enemy == 1 ||
                 stage[r_x][r_y].is_player == 1 ||
                 stage[r_x][r_y].is_wall == 1 ||
                 stage[r_x][r_y].is_item == 1);

        stage[r_x][r_y].mark = 'E';
        stage[r_x][r_y].is_enemy = 1; // アイテムの位置設定

        enemy = enemy_pick(enemies);
        stage[r_x][r_y].enemy = enemy; // アイテム情報
        (*enemy_count)++;
        printf("新しい敵が出現しました！　場の敵の数%d\n", *enemy_count);
    }
    else
        printf("場の敵の数: %d\n", *enemy_count);
    if ((*item_count) < MAX_ITEM_COUNT && *gaming % 7 == 2)
    {
        Item_effect item;
        int r_x, r_y;
        do
        {
            r_y = rand() % (LINE - 2) + 1;
            r_x = rand() % (COLUMN - 2) + 1;
            if (try_count > MAX_TRY)
            {
                break;
            }
        } while (stage[r_x][r_y].is_enemy == 1 ||
                 stage[r_x][r_y].is_player == 1 ||
                 stage[r_x][r_y].is_wall == 1 ||
                 stage[r_x][r_y].is_item == 1);

        stage[r_x][r_y].mark = 'I';
        stage[r_x][r_y].is_item = 1; // アイテムの位置設定

        item = item_pick(items);
        stage[r_x][r_y].item = item; // アイテム情報
        (*item_count)++;
        printf("新しいアイテムが出現しました！　場のアイテム数%d\n", *item_count);
    }

    else
        printf("場のアイテム数: %d\n", *item_count);
    printf("コマンドを入力してください: ");

    fgets(input, sizeof(input), stdin); // １行丸ごと読み取り
    input[strcspn(input, "\n")] = '\0'; // character spanで改行を取り除く。input[改行が出てくる位置] = ヌル

    char *token;
    token = strtok(input, " ");
    while (token != NULL) // 1.moveコマンド
    {
        if (strcmp(token, "w") == 0)
        {
            move = 1;
            if (stage[*player_x][*player_y - 1].is_wall == 1)
            {
                printf("壁で進めない！\n");
            }
            else
            {
                printf("上に移動！\n");
                (*player_y)--;
                stage[*player_x][*player_y].mark = 'P';        // プレイヤーの位置を更新
                stage[*player_x][*player_y].is_player = 1;     // プレイヤーの位置を更新
                stage[*player_x][*player_y + 1].mark = '.';    // 前の位置
                stage[*player_x][*player_y + 1].is_player = 0; // 前の位置を空にする
            }
        }
        else if (strcmp(token, "a") == 0)
        {
            move = 1;
            if (stage[*player_x - 1][*player_y].is_wall == 1)
            {
                printf("壁で進めない！\n");
            }
            else
            {
                printf("左に移動！\n");
                (*player_x)--;
                stage[*player_x][*player_y].mark = 'P';        // プレイヤーの位置を更新
                stage[*player_x][*player_y].is_player = 1;     // プレイヤーの位置を更新
                stage[*player_x + 1][*player_y].mark = '.';    // 前の位置
                stage[*player_x + 1][*player_y].is_player = 0; // 前の位置を空にする
            }
        }
        else if (strcmp(token, "s") == 0)
        {
            move = 1;
            if (stage[*player_x][*player_y + 1].is_wall == 1)
            {
                printf("壁で進めない！\n");
            }
            else
            {
                printf("下に移動！\n");
                (*player_y)++;
                stage[*player_x][*player_y].mark = 'P';        // プレイヤーの位置を更新
                stage[*player_x][*player_y].is_player = 1;     // プレイヤーの位置を更新
                stage[*player_x][*player_y - 1].mark = '.';    // 前の位置
                stage[*player_x][*player_y - 1].is_player = 0; // 前の位置を空にする
            }
        }
        else if (strcmp(token, "d") == 0)
        {
            move = 1;
            if (stage[*player_x + 1][*player_y].is_wall == 1)
            {
                printf("壁で進めない！\n");
            }
            else
            {
                printf("右に移動！\n");
                (*player_x)++;
                stage[*player_x][*player_y].mark = 'P';        // プレイヤーの位置を更新
                stage[*player_x][*player_y].is_player = 1;     // プレイヤーの位置を更新
                stage[*player_x - 1][*player_y].mark = '.';    // 前の位置
                stage[*player_x - 1][*player_y].is_player = 0; // 前の位置を空にする
            }
        }
        else
        {
            if (move == 1)
                printf("不明な入力: %s\n", token);
        }
        if (stage[*player_x][*player_y].is_enemy == 1) // 敵とぶつかった時処理
        {
            printf("敵が襲いかかってきた\n");
            int result = battle(stage, player_x, player_y, player, items, item_num); // 1：勝ち, 2：負け
            if (result == 1)
            {
                stage[*player_x][*player_y].is_enemy = 0;
                (*enemy_count)--;
            }

            if (result == 2)
            {
                print_stage(stage);
                *gaming = 0;
                printf("ーーーーーーーーーーーーーーーーーー\n\n");
                printf("　　　　　ゲームオーバー！\n\n");
                printf("ーーーーーーーーーーーーーーーーーー\n");
            }
            return;
        }

        if (stage[*player_x][*player_y].is_item == 1)
        { // アイテムを拾った時処理
            stage[*player_x][*player_y].is_item = 0;
            if (*item_num >= 10)
            {
                printf("インベントリがいっぱいです。アイテムを拾えません。\n");
                break;
            }
            printf("アイテムを拾いました！\n");
            Item_effect item = stage[*player_x][*player_y].item;
            printf("%s : \n", item.name);
            if (item.power >= 0)
                printf("power +%d\n", item.power);
            else
                printf("power %d\n", item.power);
            if (item.hp >= 0)
                printf("hp +%d\n", item.hp);
            else
                printf("hp %d\n", item.hp);
            if (item.max_hp >= 0)
                printf("max_hp +%d\n", item.max_hp);
            else
                printf("max_hp %d\n", item.max_hp);
            if (item.defense >= 0)
                printf("defense +%d\n", item.defense);
            else
                printf("defense %d\n", item.defense);
            player->inventory[*item_num] = item;
            (*item_num)++;
            (*item_count)--;
            printf("アイテムをインベントリに追加しました（コマンド'i'で表示）。\n");
        }
        token = strtok(NULL, " "); // 次のトークンへ
    }
    // 2. saveコマンド
    if (strncmp(input, "save", 4) == 0)
    {
        char filename[100];
        char *token = strtok(input + 5, " \n"); // "save " をスキップしてスペース・改行で切る
        if (token != NULL)
        {
            strcpy(filename, token);
            // fopen など処理を続ける
        }

        printf("保存コマンドが実行されました：%s\n", filename);
        printf("%s", filename);
        FILE *fp = fopen(filename, "w");
        if (fp == NULL)
        {
            printf("ファイルを開くことができませんでした。: %s\n", filename);
        }
        else
        {
            fprintf(fp, "%d\n", *item_num);
            for (int i = 0; i < LINE; i++)
            {
                for (int j = 0; j < COLUMN; j++)
                {
                    if (stage[j][i].is_item)
                    {
                        fprintf(fp, "I,%s,%d,%d,%d,%d,%d",
                                stage[j][i].item.name,
                                stage[j][i].item.power,
                                stage[j][i].item.hp,
                                stage[j][i].item.max_hp,
                                stage[j][i].item.defense,
                                stage[j][i].item.type);
                    }
                    else if (stage[j][i].is_player)
                    {
                        fprintf(fp, "P,%d,%d,%d,%d,%d,%d,",
                                player->base.level,
                                player->base.exp,
                                player->base.power,
                                player->base.hp,
                                player->base.max_hp,
                                player->base.defense);
                        for (int temp = 0; temp < *item_num - 1; temp++)
                        {
                            fprintf(fp, "%s,%d,%d,%d,%d,%d",
                                    player->inventory[temp].name,
                                    player->inventory[temp].power,
                                    player->inventory[temp].hp,
                                    player->inventory[temp].max_hp,
                                    player->inventory[temp].defense,
                                    player->inventory[temp].type);
                            if (temp < *item_num - 1)
                                fprintf(fp, ",");
                        }
                    }
                    else if (stage[j][i].is_enemy)
                    {
                        fprintf(fp, "E,%d,%d,%d,%d,%d,%d",
                                stage[j][i].enemy.level,
                                stage[j][i].enemy.exp,
                                stage[j][i].enemy.power,
                                stage[j][i].enemy.hp,
                                stage[j][i].enemy.max_hp,
                                stage[j][i].enemy.defense);
                    }
                    else // #か.のとき
                    {
                        fprintf(fp, "%c", stage[j][i].mark);
                    }
                    if (j < COLUMN - 1)
                        fprintf(fp, ",");
                }
                fprintf(fp, "\n");
            }

            fclose(fp);
            printf("ゲームの状態が保存されました。\n");
        }
    }
    // 3. quitコマンド
    else if (strncmp(input, "quit", 4) == 0)
    {
        printf("終了します。\n最終状態↓↓↓\n");
        *gaming = 0;
        return;
    }
    // 4. iコマンド
    else if (strcmp(input, "i") == 0)
    {
        printf("=== INVENTORY ===\n");
        if (*item_num == 0)
        {
            printf("インベントリは空です。\n");
        }
        else
        {
            for (int i = 0; i < *item_num; i++)
            {
                printf("%d. %s\n", i + 1, player->inventory[i].name);
            }
        }
        printf("アイテム総数: %d/10\n", *item_num);
    }
    // 5. p コマンド
    else if (strcmp(input, "p") == 0)
    {
        printf("==== PLAYER STATUS ===\n");
        printf("%sのステータス\n", player->base.name);
        printf("レベル：%d\n", player->base.level);
        printf("経験値：%d (次のレベルまであと%d)\n", player->base.level, (player->level_up - player->base.level));
        printf("攻撃力：%d\n", player->base.power);
        printf("体力：%d/%d\n", player->base.hp, player->base.max_hp);
        printf("防御力：%d\n", player->base.defense);
    }

    else
    {
        if (move == 0)
            printf("不明なコマンドです。\n");
    }
    print_stage(stage); // ステージを表示
    printf("Legend: P=Player, E=Enemy, I=Item, #=Wall, .=Empty\n");
    (*gaming)++;
}

int battle(Cell stage[][LINE], int *player_x, int *player_y, Player *player, Item_effect items[], int *item_num)
{
    int finished = 0;
    char choice[10], equipment_input[10];
    int equipment_choice, wrong;
    int is_equipped = 0, is_withsord = 0, is_withshield = 0;
    int player_power = player->base.power; // plyer_系はバトル中のパラメータ変化保存用
    int player_hp = player->base.hp;
    int player_max_hp = player->base.max_hp;
    int player_defense = player->base.defense;
    int old_power = player->base.power; // old系は前のを記憶しておくため
    int old_max_hp = player->base.max_hp;
    int old_defense = player->base.defense;
    int old_level = player->base.level;

    printf("戦闘開始！\n");
    battle_printer(stage, player_x, player_y, player);

    while (finished == 0)
    {
        wrong = 0;
        printf("何をしますか？\n");
        printf("1. 攻撃\n");
        printf("2. アイテムを装備\n");
        printf("3. アイテムを使用\n");
        printf("4. 逃げる\n");
        printf("選択肢を入力してください: ");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = '\0';

        if (strcmp(choice, "1") == 0) // 1. 攻撃
        {
            printf("攻撃を選択しました。\n");
            // プレイヤーの攻撃
            printf("<<<%sの攻撃>>>\n", player->base.name);
            printf("%sに%dのダメージ\n", stage[*player_x][*player_y].enemy.name,
                   positive_sub(player_power, stage[*player_x][*player_y].enemy.defense));

            stage[*player_x][*player_y].enemy.hp -= // E_HP減らす処理
                positive_sub(player_power, stage[*player_x][*player_y].enemy.defense);
            if (stage[*player_x][*player_y].enemy.hp <= 0) // バトル終了（勝利）
            {
                stage[*player_x][*player_y].enemy.hp = 0;
                finished = 1;
                printf("%sを倒した！\n", stage[*player_x][*player_y].enemy.name);
                printf("経験値を%d獲得！\n", stage[*player_x][*player_y].enemy.exp);
                player->base.exp += stage[*player_x][*player_y].enemy.exp;
                if (player->base.exp >= player->level_up) // 4.レベルアップ処理
                {
                    while (player->base.exp >= player->level_up)
                    {
                        player->base.power += (player->base.level * player->base.level); // パワーはレベルアップ前のレベルの2乗分付与
                        player->base.max_hp += pow(2, player->base.level);               // 最大HPは2のレベルアップ前のレベル乗分付与
                        player->base.defense += (player->base.level * 2);                // 防御力はレベルアップ前のレベルの2倍分付与
                        player->base.level++;
                        player->level_up += pow(player->base.level, 3); // 次の到達点は次のレベルの3乗
                    }
                    printf("装備が壊れ、アイテムの効果も切れた!　%sのステータスはもとに戻った\n", player->base.name);
                    printf("〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜\n");
                    printf("%sはレベルが%dから%dに上がった！\n", player->base.name, old_level, player->base.level);
                    printf("パワー：%d→%d\n", old_power, player->base.power);
                    printf("最大HP：%d→%d\n", old_max_hp, player->base.max_hp);
                    printf("防御力：%d→%d\n", old_defense, player->base.defense);
                    printf("〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜〜\n");
                }

                else
                    printf("現在の経験値：%d\n", player->base.exp);
                return 1;
            }
            // 敵の攻撃
            printf("<<<%sの攻撃>>>\n", stage[*player_x][*player_y].enemy.name);
            printf("%sに%dのダメージ\n", player->base.name,
                   positive_sub(stage[*player_x][*player_y].enemy.power, player_defense));

            player_hp -= positive_sub(stage[*player_x][*player_y].enemy.power, player_defense); // P_HP減らす処理
            if (player_hp <= 0)                                                                 // バトル終了（敗北）
            {
                finished = 2;
                player_hp = 0;
                printf("%sに倒された！\n", stage[*player_x][*player_y].enemy.name);
                break;
            }
        }

        else if (strcmp(choice, "2") == 0) // 2. アイテムを装備
        {
            if (*item_num == 0)
            {
                printf("使用できるアイテムがありません。\n");
                wrong = 1;
            }
            else
                printf("アイテムを装備します。装備するアイテムを選んでください。(番号を入力)\n");

            if (wrong != 1)
            {
                Item_effect use_item;
                for (int i = 0; i < *item_num; i++)
                {
                    printf("%d. %s\n", i + 1, player->inventory[i].name);
                }

                while (1)
                {
                    fgets(equipment_input, sizeof(equipment_input), stdin);
                    equipment_choice = atoi(equipment_input) - 1; // 1始まり→0始まり
                    if (equipment_choice >= 0 && equipment_choice < *item_num)
                    {
                        use_item = player->inventory[equipment_choice];
                        break;
                    }
                    else
                        printf("アイテムが見つかりませんでした。\n");
                }
                if (use_item.type != 1 && use_item.type != 2 && use_item.type != 3)
                {
                    printf("このタイプのアイテムは装備できません。\n");
                    wrong = 1;
                }
                else if (is_equipped == 1 && use_item.type == 3)
                {
                    printf("すでに別のものが装備されています。\n");
                    wrong = 1;
                }
                else if (is_withshield == 1 && use_item.type == 2)
                {
                    printf("すでに別の盾を持っています。\n");
                    wrong = 1;
                }
                else if (is_withsord == 1 && use_item.type == 1)
                {
                    printf("すでに別の武器を持っています。\n");
                    wrong = 1;
                }
                if (wrong != 1)
                {
                    printf("%sを装備します...\n", use_item.name);

                    player_power = positive_sum(player_power, use_item.power);
                    player_hp = positive_sum(player_hp, use_item.hp);
                    player_max_hp = positive_sum(player_max_hp, use_item.max_hp);
                    player_defense = positive_sum(player_defense, use_item.defense);
                    if (player_hp > player_max_hp)
                        player_hp = player_max_hp;
                    if (player_hp <= 0 || player_max_hp <= 0)
                    {
                        player_hp = player_max_hp = 1;
                    }

                    for (int i = equipment_choice; i < *item_num - 1; i++)
                    {
                        player->inventory[i] = player->inventory[i + 1]; // 1つ前に詰める
                    }

                    (*item_num)--;
                    if (use_item.type == 3)
                        is_equipped = 1;
                    else if (use_item.type == 2)
                        is_withshield = 1;
                    else if (use_item.type == 1)
                        is_withsord = 1;
                    printf("装備が完了しました!\n");
                    if (use_item.power >= 0)
                        printf("power %d (+%d)\n", player_power, use_item.power);
                    else
                        printf("power %d (%d)\n", player_power, use_item.power);
                    if (use_item.hp >= 0)
                        printf("hp %d (+%d)\n", player_hp, use_item.hp);
                    else
                        printf("hp %d (%d)\n", player_hp, use_item.hp);
                    if (use_item.max_hp >= 0)
                        printf("max_hp %d (+%d)\n", player_max_hp, use_item.max_hp);
                    else
                        printf("max_hp %d (%d)\n", player_max_hp, use_item.max_hp);
                    if (use_item.defense >= 0)
                        printf("defense %d (+%d)\n", player_defense, use_item.defense);
                    else
                        printf("defense %d (%d)\n", player_defense, use_item.defense);
                }
            }
        }

        else if (strcmp(choice, "3") == 0) // 3. アイテムを使用
        {
            if (*item_num == 0)
            {
                printf("使用できるアイテムがありません。\n");
                wrong = 1;
            }
            else
                printf("アイテムを使用します。使用するアイテムを選んでください。(番号を入力)\n");

            if (wrong != 1)
            {
                Item_effect use_item;
                for (int i = 0; i < *item_num; i++)
                {
                    printf("%d. %s\n", i + 1, player->inventory[i].name);
                }

                while (1)
                {
                    fgets(equipment_input, sizeof(equipment_input), stdin);
                    equipment_choice = atoi(equipment_input) - 1; // 1始まり→0始まり
                    if (equipment_choice >= 0 && equipment_choice < *item_num)
                    {
                        use_item = player->inventory[equipment_choice];
                        break;
                    }
                    else
                        printf("アイテムが見つかりませんでした。\n");
                }
                if (use_item.type == 0)
                {
                    printf("%sを使用します...\n", use_item.name);

                    player_power = positive_sum(player_power, use_item.power);
                    player_hp = positive_sum(player_hp, use_item.hp);
                    player_max_hp = positive_sum(player_max_hp, use_item.max_hp);
                    player_defense = positive_sum(player_defense, use_item.defense);
                    if (player_hp > player_max_hp)
                        player_hp = player_max_hp;
                    if (player_hp <= 0 || player_max_hp <= 0)
                    {
                        player_hp = player_max_hp = 1;
                    }

                    for (int i = equipment_choice; i < *item_num - 1; i++)
                    {
                        player->inventory[i] = player->inventory[i + 1]; // 1つ前に詰める
                    }

                    (*item_num)--;
                    printf("アイテムを使用しました!\n");
                    if (use_item.power >= 0)
                        printf("power %d (+%d)\n", player_power, use_item.power);
                    else
                        printf("power %d (%d)\n", player_power, use_item.power);
                    if (use_item.hp >= 0)
                        printf("hp %d (+%d)\n", player_hp, use_item.hp);
                    else
                        printf("hp %d (%d)\n", player_hp, use_item.hp);
                    if (use_item.max_hp >= 0)
                        printf("max_hp %d (+%d)\n", player_max_hp, use_item.max_hp);
                    else
                        printf("max_hp %d (%d)\n", player_max_hp, use_item.max_hp);
                    if (use_item.defense >= 0)
                        printf("defense %d (+%d)\n", player_defense, use_item.defense);
                    else
                        printf("defense %d (%d)\n", player_defense, use_item.defense);
                }
                else
                {
                    printf("これは装備するものです。\n");
                    wrong = 1;
                }
            }
        }

        else if (strcmp(choice, "4") == 0) // 4. 逃げる
        {
            if (chance_n_percent(stage[*player_x][*player_y].enemy.level))
            {                                   // 戦闘終了
                printf("うまく逃げ切れた。\n"); // 逃げれる確率は(10-敵レベル)*10%
                finished = 3;
                break;
            }
            else
            {
                printf("逃げられなかった！\n");
                // 敵の攻撃
                printf("<<<%sの攻撃>>>\n", stage[*player_x][*player_y].enemy.name);
                printf("%sに%dのダメージ\n", player->base.name,
                       positive_sub(stage[*player_x][*player_y].enemy.power, player_defense));

                player_hp -= positive_sub(stage[*player_x][*player_y].enemy.power, player_defense); // P_HP減らす処理
                if (player_hp <= 0)                                                                 // バトル終了（敗北）
                {
                    finished = 2;
                    player_hp = 0;
                    printf("%sに倒された！\n", player->base.name);
                    break;
                }
            }
        }
        else
            printf("無効な選択です。\n");
        if (wrong != 1 && finished == 0)
            battle_printer(stage, player_x, player_y, player);
    }
    return finished;

    // 戦闘のロジックをここに実装
    // 例: プレイヤーと敵のHP、攻撃力などを定義し、戦闘の進行をシミュレートする
    // 戦闘が終了したら、プレイヤーの位置やステージの状態を更新する
    /*
    ーーーーーーーーーーーーーーーーーーーーーーーーーー
    |   Level：ーーー     HP：ーーー/ーーー
    |   Name：ーーー
    |
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    |   Level：ーーー　　　HP：ーーー/ーーー
    |   Name：ーーー
    |
    ーーーーーーーーーーーーーーーーーーーーーーーーーー
    */
}

void battle_printer(Cell stage[][LINE], int *player_x, int *player_y, Player *player)
{
    printf("ーーーーーーーーーーーーーーーーーーーーーーーーーー\n");
    printf("|   Level：%d     HP：%d/%d\n", stage[*player_x][*player_y].enemy.level, stage[*player_x][*player_y].enemy.hp, stage[*player_x][*player_y].enemy.max_hp);
    printf("|   Name：%s\n", stage[*player_x][*player_y].enemy.name);
    printf("|\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("|   Level：%d　　　HP：%d/%d\n", player->base.level, player->base.hp, player->base.max_hp);
    printf("|   Name：%s\n", player->base.name);
    printf("|\n");
    printf("ーーーーーーーーーーーーーーーーーーーーーーーーーー\n");
}