
#include <stdio.h>
#include <stdlib.h>

#define value 4
#define enemy_value 6

struct cell
{
    int box, path, goal, delay, tunnel, enemy_path, enemy_goal;
};

struct bot
{
    int x, y, f_a, f_r, a_a;
};

struct pos
{
    int x, y;
};

void fill_map(struct cell** map, char* line, int i, int j, int* box_number);
void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w, int coef, int* min_tick);
void fill_feature(struct cell** map, struct bot* Deidara, char ent_type, int x, int y, int coef);
void fill_bomb(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int* min_tick);
int fill_bomb_dir(struct cell** map, int x, int y, int param_1, int less_than);
void fill_path(struct cell** map, struct bot* Deidara, int h, int w);
void get_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb, int deep);
void fill_goal(struct cell** map, struct bot* Deidara, int h, int w);
void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w);
void final_map(struct cell** map, struct bot* Deidara, int h, int w, struct pos* aim, int min_tick);
void find_dir(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir);
void freentf(struct cell** map, int h, int w, struct pos* aim, struct pos* enemy_aim);
void fill_enemy_box(struct cell** map, struct bot* Saken, int y, int x, int h, int w);
void fill_enemy_goal(struct cell** map, struct bot* Saken, int h , int w);
void fill_enemy_path(struct cell** map, struct bot* Saken, int h, int w);
void get_enemy_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb, int deep);
void final_enemy_map(struct cell** map, struct bot* Saken, int h, int w, struct pos* enemy_aim, int min_tick);
int my_abs_dif(int n1, int n2);

int main(void)
{
    struct bot* Deidara = malloc(sizeof(struct bot));
    Deidara->f_r = 2;
    Deidara->a_a = 1;
    struct bot* Saken = malloc(sizeof(struct bot));
    Saken->f_r = 2;
    Saken->a_a = 1;
    int box_number = 0;
    while (1)
    {
        int h, w, player_id, tick;
        scanf("%d%d%d%d", &w, &h, &player_id, &tick);
        fprintf(stderr, "h - %d, w - %d, player_id - %d, tick - %d\n", h, w, player_id, tick);

        // read map
        struct cell** map = malloc(sizeof(struct cell*) * h);
        int current_number = 0;
        for (int i = 0; i < h; i++)
        {
            map[i] = malloc(sizeof(struct cell) * w);
            char* line = malloc(sizeof(char) * (w + 1));
            scanf("%s", line);
            for (int j = 0; j < w; j++)
            {
                if (tick == 1)
                    fill_map(map, line, i, j, &box_number);
                else
                    fill_map(map, line, i, j, &current_number);
            }
            free(line);
        }

        // number of entities
        int n;
        scanf("%d", &n);
        getchar();
        fprintf(stderr, "number of entities - %d\n", n);
        int coef = (current_number * 100 / box_number) + 10;
        int min_tick = 0;
        // read entities
        for (int i = 0; i < 2 * (n - 1) + 1; i++)
        {
            char ent_type;
            int p_id, x, y, param_1, param_2;
            scanf("%c", &ent_type);
            if (ent_type == 'f')
            {
                scanf("%c", &ent_type);
                scanf("%c", &ent_type);
                if (ent_type == 'r')
                    ent_type = 'l';
            }
            scanf("%d%d%d%d%d", &p_id, &x, &y, &param_1, &param_2);
            fill_entity(map, Deidara, Saken, ent_type, p_id, x, y, param_1, param_2, player_id, h, w, coef, &min_tick);
        }

        fill_goal(map, Deidara, h, w);
        fill_enemy_goal(map, Saken, h, w);

        fill_path(map, Deidara, h, w);
        fill_enemy_path(map, Saken, h, w);

        struct pos* aim = malloc(sizeof(struct pos));
        aim->y = Deidara->y;
        aim->x = Deidara->x;
        fprintf(stderr, "Min tick %d\n", min_tick);
        final_map(map, Deidara, h, w, aim, min_tick);
        
        struct pos* enemy_aim = malloc(sizeof(struct pos));
        enemy_aim->y = Saken->y;
        enemy_aim->x = Saken->x;
        final_enemy_map(map, Saken, h, w, enemy_aim, min_tick);

        int dir = 5;
        fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
        find_dir(map, Deidara, aim, h, w, &dir);

        const char actions[][10] = { "left", "right", "up", "down", "bomb", "stay" };

        // bot action
        printf("%s\n", actions[dir]);
        fflush(stdout);

        freentf(map, h, w, aim, enemy_aim);
    }
    free(Saken);
    free(Deidara);
    return 0;
}

void fill_map(struct cell** map, char* line, int i, int j, int* box_number)
{
    switch (line[j])
    {
    case '.':
        map[i][j].box = 1;
        break;

    case ';':
        map[i][j].box = 10;
        (*box_number)++;
        break;

    case '!':
        map[i][j].box = 100;
        break;
    default:
        break;
    }

    map[i][j].path = 1000;
    map[i][j].goal = 0;
    map[i][j].delay = 0;
    map[i][j].enemy_path = 1000;
    map[i][j].enemy_goal = 0;
}

void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w, int coef, int* min_tick)
{
    switch (ent_type)
    {
    case 'p':
        if (p_id == player_id)
        {
            Deidara->x = x;
            Deidara->y = y;
            Deidara->f_a = param_1;
            Deidara->f_r = param_2;
            fprintf(stderr, "Deidara: x - %d, y - %d, magazine - %d, power = %d\n", Deidara->x, Deidara->y, Deidara->f_a, Deidara->f_r);
        }
        else
        {
            Saken->x = x;
            Saken->y = y;
            Saken->f_a = param_1;
            Saken->f_r = param_2;
            fprintf(stderr, "Saken: x - %d, y - %d, magazine - %d, power = %d\n", Saken->x, Saken->y, Saken->f_a, Saken->f_r);
        }
        break;

    case 'b':
        fill_bomb(map, Deidara, Saken, p_id, x, y, param_1, param_2, h, w, player_id, min_tick);
        fprintf(stderr, "bomb: x - %d, y - %d, tick - %d, radius - %d\n", x, y, param_1, param_2);
        break;

    case 'a':
        fill_feature(map, Deidara, ent_type, x, y, coef);
        fprintf(stderr, "Feature amount: x - %d, y - %d, tick - %d, radius - %d\n", x, y, param_1, param_2);
        break;

    case 'l':
        fill_feature(map, Deidara, ent_type, x, y, coef);
        fprintf(stderr, "Feature radius: x - %d, y - %d, tick - %d, radius - %d\n", x, y, param_1, param_2);
        break;

    default: break;
    }
}

void fill_bomb(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int* min_tick)
{
    int less_than = 5 - Deidara->f_r;
    

    if (player_id == p_id && Deidara->f_a == 0)
    {
        if (*min_tick == 0 || *min_tick > param_1)
            *min_tick = param_1;
    }

    if (param_1 >= less_than)
    {
        map[y][x].box = 50;
    }
    else
        map[y][x].box = -1 * param_1;
    map[y][x].goal = -150;

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;
        if (!fill_bomb_dir(map, tmp_x, y, param_1, less_than)) break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;
        if (!fill_bomb_dir(map, tmp_x, y, param_1, less_than)) break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;
        if (!fill_bomb_dir(map, x, tmp_y, param_1, less_than)) break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;
        if (!fill_bomb_dir(map, x, tmp_y, param_1, less_than)) break;
    }
}

int fill_bomb_dir(struct cell** map, int x, int y, int param_1, int less_than)
{
    if (map[y][x].box == 1)
    {
        if (param_1 >= less_than)
            map[y][x].goal = -150;
        map[y][x].box = -1 * param_1;
    }
    else if (map[y][x].box < 0)
    {
        if (map[y][x].box < -1 * param_1)
            map[y][x].box = -1 * param_1;
    }
    else if (map[y][x].box == 10)
    {
        if (param_1 >= less_than)
        {
            map[y][x].box = 50;
        }
        else
            map[y][x].box = -1 * param_1;
        return 0;
    }
    else
        return 0;
    return 1;
}

void fill_feature(struct cell** map, struct bot* Deidara, char ent_type, int x, int y, int coef)
{
    int a = 0;
    if (Deidara->f_a > Deidara->a_a)
        Deidara->a_a = Deidara->f_a;
    //fprintf(stderr, "COEFF and a_a %d %d \n", 8 * coef / 100, Deidara->a_a);
    if (ent_type == 'a')
    {
        if (Deidara->a_a == 1)
            map[y][x].goal += 10 * coef / 100;
    }

    if (ent_type == 'l')
    {
        if (Deidara->f_r == 2 || Deidara->f_r == 3)
            map[y][x].goal += 10 * coef / 100;
    }
}

void fill_goal(struct cell** map, struct bot* Deidara, int h, int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].box == 10)
                fill_box(map, Deidara, i, j, h, w);
        }
    }
}

void fill_enemy_goal(struct cell** map, struct bot* Saken, int h , int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].box == 10)
                fill_enemy_box(map, Saken, i, j, h, w);
        }
    }
}

void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w)
{
    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal += value;
        }
        else
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal += value;
        }
        else
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;

        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].goal += value;
        }
        else
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;

        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].goal += value;
        }
        else
            break;
    }
}

void fill_enemy_box(struct cell** map, struct bot* Saken, int y, int x, int h, int w)
{
    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].enemy_goal += enemy_value;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].enemy_goal += enemy_value;
        }
        else
            break;
    }

    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;

        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].enemy_goal += enemy_value;
        }
        else
            break;
    }

    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;

        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].enemy_goal += enemy_value;
        }
        else
            break;
    }
}

void fill_path(struct cell** map, struct bot* Deidara, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Deidara->x;
    int y = Deidara->y;
    int bomb = 0;
    int deep = 0;
    get_point(map, y, x, step, prev, h, w, bomb, deep);
}

void get_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb, int deep)
{
    //if (step >= 30) return;
    if (step == 0)
    {
        map[y][x].path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box == 1 && step < map[y][x].path)
    {
        map[y][x].path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box < 0 && step < map[y][x].path)
    {
        bomb++;
        if (map[y][x].box != -1 * step)
        {
            map[y][x].path = step++;
            map[y][x].delay = 0;
        }
        else
        {
            step += bomb;
            map[y][x].path = step++;
            map[y][x].delay = bomb;
        }
    }
    else
        return;

    if (prev != 'L' && x + 1 < w)
        get_point(map, y, x + 1, step, 'R', h, w, bomb, deep);
    if (prev != 'U' && y + 1 < h)
        get_point(map, y + 1, x, step, 'D', h, w, bomb, deep);
    if (prev != 'R' && x - 1 >= 0)
        get_point(map, y, x - 1, step, 'L', h, w, bomb, deep);
    if (prev != 'D' && y - 1 >= 0)
        get_point(map, y - 1, x, step, 'U', h, w, bomb, deep);
}

void fill_enemy_path(struct cell** map, struct bot* Saken, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Saken->x;
    int y = Saken->y;
    int bomb = 0;
    int deep = 0;
    get_enemy_point(map, y, x, step, prev, h, w, bomb, deep);
}

void get_enemy_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb, int deep)
{
    if (step == 0)
    {
        map[y][x].enemy_path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box == 1 && step < map[y][x].enemy_path)
    {
        map[y][x].enemy_path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box < 0 && step < map[y][x].enemy_path)
    {
        bomb++;
        if (map[y][x].box != -1 * step)
        {
            map[y][x].enemy_path = step++;
            map[y][x].delay = 0;
        }
        else
        {
            step += bomb;
            map[y][x].enemy_path = step++;
            map[y][x].delay = bomb;
        }
    }
    else
        return;

    if (prev != 'L' && x + 1 < w)
        get_enemy_point(map, y, x + 1, step, 'R', h, w, bomb, deep);
    if (prev != 'U' && y + 1 < h)
        get_enemy_point(map, y + 1, x, step, 'D', h, w, bomb, deep);
    if (prev != 'R' && x - 1 >= 0)
        get_enemy_point(map, y, x - 1, step, 'L', h, w, bomb, deep);
    if (prev != 'D' && y - 1 >= 0)
        get_enemy_point(map, y - 1, x, step, 'U', h, w, bomb, deep);
}

void final_map(struct cell** map, struct bot* Deidara, int h, int w, struct pos* aim, int min_tick)
{
    if (map[Deidara->y][Deidara->x].goal == 0)
        map[Deidara->y][Deidara->x].goal = -135;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].goal == 0)
                map[i][j].goal = -135;
            if (min_tick != 0)
            {
                if (min_tick <= map[i][j].path)
                    map[i][j].goal -= (map[i][j].path - min_tick);
            }
            else
                map[i][j].goal -= map[i][j].path; //+ up to tick or path
            if (map[i][j].goal > map[aim->y][aim->x].goal)
            {
                aim->y = i;
                aim->x = j;
                //fprintf(stderr, "aim changed to %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
            }
            else if (map[i][j].goal == map[aim->y][aim->x].goal)
            {
                int middle_x = w / 2;
                int middle_y = h / 2;
                int aim_dif = my_abs_dif(aim->y, middle_y) + my_abs_dif(aim->x, middle_x);
                int cur_dif = my_abs_dif(i, middle_y) + my_abs_dif(j, middle_x);
                if (cur_dif < aim_dif)
                {
                    aim->y = i;
                    aim->x = j;
                    //fprintf(stderr, "aim changed to %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                }
            }
        }
    }
}

void final_enemy_map(struct cell** map, struct bot* Saken, int h, int w, struct pos* enemy_aim, int min_tick)
{
    if (map[Saken->y][Saken->x].enemy_goal == 0)
        map[Saken->y][Saken->x].enemy_goal = -135;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].enemy_goal == 0)
                map[i][j].enemy_goal = -135;
            if (min_tick != 0)
            {
                if (min_tick <= map[i][j].enemy_path)
                    map[i][j].enemy_goal -= (map[i][j].enemy_path - min_tick);
            }
            else
                map[i][j].enemy_goal -= map[i][j].enemy_path; //+ up to tick or path
            if (map[i][j].enemy_goal > map[enemy_aim->y][enemy_aim->x].enemy_goal)
            {
                enemy_aim->y = i;
                enemy_aim->x = j;
                //fprintf(stderr, "aim changed to %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
            }
            else if (map[i][j].enemy_goal == map[enemy_aim->y][enemy_aim->x].enemy_goal)
            {
                int middle_x = w / 2;
                int middle_y = h / 2;
                int aim_dif = my_abs_dif(enemy_aim->y, middle_y) + my_abs_dif(enemy_aim->x, middle_x);
                int cur_dif = my_abs_dif(i, middle_y) + my_abs_dif(j, middle_x);
                if (cur_dif < aim_dif)
                {
                    enemy_aim->y = i;
                    enemy_aim->x = j;
                    fprintf(stderr, "aim changed to %d %d %d\n", map[enemy_aim->y][enemy_aim->x].goal, enemy_aim->y, enemy_aim->x);
                }
            }
        }
    }
}

void find_dir(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir)
{
    int anti_timeout = 0;
    int y = aim->y;
    int x = aim->x;
    int wait = 0;
    struct pos next = { -1, -1 };

    if (y == Deidara->y && x == Deidara->x)
    {
        fprintf(stderr, "Bomb plant case 1 %d %d \n", y, x);
        *dir = 4;
        return;
    }
    else if ((map[aim->y][aim->x].path >= 5 && map[aim->y][aim->x].path > Deidara->f_r) && map[Deidara->y][Deidara->x].goal == value && Deidara->f_a > 0)
    {
        fprintf(stderr, "Bomb plant case 2 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if (map[aim->y][aim->x].path > Deidara->f_r && map[Deidara->y][Deidara->x].goal == value * 2)
    {
        fprintf(stderr, "Bomb plant case 3 %d %d %d\n", aim->y, aim->x, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if (map[aim->y][aim->x].path > Deidara->f_r && map[Deidara->y][Deidara->x].goal == value && Deidara->f_a > 1)
    {
        fprintf(stderr, "Bomb plant case 4 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }

    while (y != Deidara->y || x != Deidara->x)
    {
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 1;
                //x--;
                //continue;
            }
        }
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 3;
                //y--;
                //continue;
            }
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 0;
                //x++;
                //continue;
            }
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 2;
                //y++;
                //continue;
            }
        }
        if (*dir == 1) x--;
        if (*dir == 3) y--;
        if (*dir == 0) x++;
        if (*dir == 2) y++;
        //fprintf(stderr, "Dir is %d\n", *dir);
        if (anti_timeout < 50) anti_timeout++;
        else if (anti_timeout >= 50)
        {
            //*dir = rand() % 4; IMPORTANT FOR REAL ARENA BOT
            fprintf(stderr, "Find_dir timeout.\n");
            *dir = 5;
            break;
        }
    }

    if (*dir != 4 && *dir != 5 && map[aim->y][aim->x].box < 0)
    {
        if (map[next.y][next.x].box < 0)
            *dir = 5;
    }

    if (wait != 0 && wait == -map[next.y][next.x].box)
    {
        *dir = 5;
    }
}

void freentf(struct cell** map, int h, int w, struct pos* aim, struct pos* enemy_aim)
{
    // print maps
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "box: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%4d ", map[i][j].box);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "path: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%4d ", map[i][j].path);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "goal: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%5d ", map[i][j].goal);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "enemy_path: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%5d ", map[i][j].enemy_path);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "enemy_goal: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%5d ", map[i][j].enemy_goal);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    //free part
    for (int i = 0; i < w; i++)
    {
        free(map[i]);
    }
    free(enemy_aim);
    free(aim);
    free(map);
}

int my_abs_dif(int n1, int n2)
{
    int res = n1 - n2;
    if (res < 0) return -res;
    return res;
}