#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
/*
to do
アイテム情報を記憶
playerのパラメータを記憶
敵のパラメータを記憶
バトルフェーズの記述
ゴールの設定
*/
#define LINE 7
#define COLUMN 20
#define P_LOCATION_X 1
#define P_LOCATION_Y 1
#define E_LOCATION_X 6
#define E_LOCATION_Y 2
#define MAX_TRY 1000
#define MAX_ITEM_COUNT 5

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
    int level;
    int exp;
    int power;
    int hp;
    int max_hp;
    int defense;
} Player;

typedef struct
{
    char mark;
    int is_wall;
    int is_player;
    int is_enemy;
    int is_item;
    Item_effect item;
} Cell;

void stage_init(Cell stage[][LINE]);
void print_stage(Cell stage[][LINE]);
void game_play(Cell stage[][LINE], int *player_x, int *player_y, int *gaming, Player *player, int *item_num, char inventory[][32], Item_effect items[], int *item_count);
void battle(Cell stage[][LINE], int *player_x, int *player_y);
Item_effect item_pick(Item_effect item[]);

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
    Player player;
    char inventory[15][32];
    int item_num = 0, item_count = 0;
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
        for (int i = 0; i < LINE; i++)
        {
            for (int j = 0; j < COLUMN; j++)
            {
                int ch;
                do
                {
                    ch = fgetc(fp); // 改行スキップのために do-while にする
                } while (ch == '\n' || ch == '\r');
                stage[j][i].mark = (char)ch;
                stage[j][i].is_wall = (ch == '#');
                stage[j][i].is_player = (ch == 'P');
                stage[j][i].is_enemy = (ch == 'E');
                stage[j][i].is_item = (ch == 'I');
                if (ch == 'P')
                {
                    player_x = j;
                    player_y = i;
                }
            }
        }
        fclose(fp);
        printf("=== GAME WORLD LOADED ===\n");
        printf("World size: %d×%d\n", LINE, COLUMN);
        printf("Player location: (%d, %d)\n", player_x, player_y);
        while (gaming)
        {
            game_play(stage, &player_x, &player_y, &gaming, &player, &item_num, inventory, items, &item_count);
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
        stage_init(stage);
        stage[P_LOCATION_X][P_LOCATION_Y].mark = 'P'; // プレイヤーの位置設定
        stage[P_LOCATION_X][P_LOCATION_Y].is_player = 1;
        stage[E_LOCATION_X][E_LOCATION_Y].mark = 'E'; // 敵の位置設定
        stage[E_LOCATION_X][E_LOCATION_Y].is_enemy = 1;

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
            game_play(stage, &player_x, &player_y, &gaming, &player, &item_num, inventory, items, &item_count);
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

void game_play(Cell stage[][LINE], int *player_x, int *player_y, int *gaming, Player *player, int *item_num, char inventory[][32], Item_effect items[], int *item_count)
{
    char input[50];
    int move = 0;
    int try_count = 0;

    if ((*item_count) < MAX_ITEM_COUNT && *gaming % 3 == 2)
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
        if (stage[*player_x][*player_y].is_enemy == 1)
        { // 敵とぶつかった時処理
            printf("敵が襲いかかってきた\n");
            battle(stage, player_x, player_y);
            break;
        }
        if (stage[*player_x][*player_y].is_item == 1)
        { // アイテムを拾った時処理
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
            strcpy(inventory[*item_num], item.name);
            (*item_num)++;
            printf("アイテムをインベントリに追加しました（コマンド'i'で表示）。\n");
            player->power += item.power;
            player->hp += item.hp;
            player->max_hp += item.max_hp;
            player->defense += item.defense;
            stage[*player_x][*player_y].is_item = 0;
            (*item_count)--;
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
            for (int i = 0; i < LINE; i++)
            {
                for (int j = 0; j < COLUMN; j++)
                {
                    fputc(stage[j][i].mark, fp);
                }
                fputc('\n', fp); // 改行を追加
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
    // 4. i または l 単独コマンド
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
                printf("%d. %s\n", i + 1, inventory[i]);
            }
        }
        printf("アイテム総数: %d/10\n", *item_num);
    }
    // 5. l コマンド
    else if (strcmp(input, "l") == 0)
    {
        printf("l コマンドが実行されました。\n");
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

void battle(Cell stage[][LINE], int *player_x, int *player_y)
{
    printf("戦闘開始！\n");
    // 戦闘のロジックをここに実装
    // 例: プレイヤーと敵のHP、攻撃力などを定義し、戦闘の進行をシミュレートする
    // 戦闘が終了したら、プレイヤーの位置やステージの状態を更新する
}