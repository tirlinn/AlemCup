#include <stdio.h>
#include <stdlib.h>

#define value 4
#define enemy_value 4

struct cell
{
    int box, path, goal, delay, tunnel, enemy_tunnel, enemy_delay, enemy_path, enemy_goal, features;
};

struct bot
{
    int x, y, f_a, f_r, a_a, f_t, f_j;
};

struct pos
{
    int x, y;
};

struct situation
{
    int assassin, trapped, half_game, p_sector;
};

void fill_map(struct cell** map, char* line, int i, int j, int* box_number, struct situation* status, int* sector_count, int h, int w);

void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w, int coef, int* min_tick);
void fill_bomb(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int* min_tick);
int fill_bomb_dir(struct cell** map, int x, int y, int param_1, int less_than);
void fill_feature(struct cell** map, struct bot* Deidara, char ent_type, int x, int y, int coef);

void read_features(struct cell** map, struct bot* Deidara, struct bot* Saken, int player_id, int p_id, int type);

void fill_goal(struct cell** map, struct bot* Deidara, int h, int w, struct situation* status);
void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w, struct situation* status);
void quadrant_priority(struct cell** map, struct situation* status, int i, int j, int h, int w);

void fill_enemy_goal(struct cell** map, struct bot* Saken, int h, int w);
void fill_enemy_box(struct cell** map, struct bot* Saken, int y, int x, int h, int w);

void fill_path(struct cell** map, struct bot* Deidara, int h, int w);
void get_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb);

void fill_features(struct cell** map, struct bot* Deidara, int h, int w);
void get_features(struct cell** map, int y, int x, int step, int prev, int h, int w, int features);

void fill_tunnel(struct cell** map, struct bot* Saken, int h, int w);
void get_tunnels(struct cell** map, struct bot* Saken, int y, int x, int deep, int h, int w);

void fill_enemy_path(struct cell** map, struct bot* Saken, int h, int w);
void get_enemy_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb);

void fill_enemy_tunnel(struct cell** map, struct bot* Saken, int h, int w);
void get_enemy_tunnels(struct cell** map, struct bot* Saken, int y, int x, int deep, int h, int w);

void final_enemy_map(struct cell** map, struct bot* Saken, int h, int w, struct pos* enemy_aim);

void adjust_priority(struct cell** map, struct bot* Deidara, struct bot* Saken, struct situation* status, struct pos* enemy_aim, int h, int w);
void adjust_box(struct cell** map, struct bot* Deidara, struct pos* enemy_aim, int y, int x, int h, int w, struct situation* status);
int adjust_one_way(struct cell** map, struct situation* status, struct pos* enemy_aim, int y, int x, int h, int w);
void adjust_quadrant(struct cell** map, struct situation* status, int y, int x, int h, int w);

void final_map(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct pos* aim, struct pos* enemy_aim, int* min_tick);

int kill_bot(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* kill_aim, struct pos* enemy_aim, int h, int w);
int find_trap_pos(struct cell** map, struct pos* tmp_aim, int y, int x, int fin_value, int h, int w, int* anti_timeout);

void find_dir(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir);
int target_acqured(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* aim, struct pos* enemy_aim, int h, int w, int* dir);
int get_out(struct cell** map, struct bot* Deidara, int* dir, int h, int w, int tick);

void print_map(struct cell** map, int h, int w, int map_index);
void freentf(struct cell** map, int h, int w, struct pos* aim, struct pos* enemy_aim, struct pos* kill_aim, int* sector_count, struct pos* tp_aim);
int my_abs_dif(int n1, int n2);
int my_abs(int n);


int main(void)
{
    struct bot* Deidara = malloc(sizeof(struct bot));
    Deidara->f_r = 2;
    Deidara->a_a = 1;
    Deidara->f_t = 0;
    Deidara->f_j = 0;
    struct bot* Saken = malloc(sizeof(struct bot));
    Saken->f_r = 2;
    Saken->a_a = 1;
    Saken->x = -1;
    Saken->f_t = 0;
    Saken->f_j = 0;
    int box_number = 0;
    struct situation* status = malloc(sizeof(struct situation));
    status->half_game = 0;
    status->p_sector = -1;
    status->assassin = 0;
    status->trapped = 0;
    while (1)
    {
        int h, w, player_id, tick;
        scanf("%d%d%d%d", &w, &h, &player_id, &tick);
        fprintf(stderr, "h - %d, w - %d, player_id - %d, tick - %d\n", h, w, player_id, tick);

        /*
        Read map
        */
        struct cell** map = malloc(sizeof(struct cell*) * h);
        int current_number = 0;
        int* sector_count = malloc(sizeof(int) * 4);
        for (int i = 0; i < 4; i++)
        {
            sector_count[i] = 0;
        }
        for (int i = 0; i < h; i++)
        {
            map[i] = malloc(sizeof(struct cell) * w);
            char* line = malloc(sizeof(char) * (w + 1));
            scanf("%s", line);
            for (int j = 0; j < w; j++)
            {
                if (tick == 1)
                    fill_map(map, line, i, j, &box_number, status, sector_count, h, w);
                else
                    fill_map(map, line, i, j, &current_number, status, sector_count, h, w);
            }
            free(line);
        }

        /*
        Quadrant priority
        */
        if ((current_number * 100 / box_number) <= 50 && tick > 1)
        {
            status->half_game = 1;
        }
        if (sector_count[status->p_sector] == 0)
        {
            status->p_sector = -1;
        }
        if (status->half_game == 1 && status->p_sector == -1)
        {
            status->p_sector = 0;
            for (int i = 1; i < 4; i++)
            {
                if (sector_count[i] > sector_count[status->p_sector])
                {
                    status->p_sector = i;
                }
            }
        }

        /*
        Number of entities
        */
        int n;
        scanf("%d", &n);
        getchar();
        fprintf(stderr, "Number of entities - %d\n", n);

        /*
        Read entities
        */
        int min_tick = 0;
        int coef = (current_number * 100 / box_number) + 10;
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
        fprintf(stderr, "After fill_entity\n");
        print_map(map, h, w, 0);

        /*
        Read features
        */
        int f_n = 0;
        scanf("%d", &f_n);
        fprintf(stderr, "Number of features - %d\n", f_n);
        getchar();
        for (int i = 0; i < f_n; i++) {
            int p_id, type;
            scanf("%d%d", &p_id, &type);
            read_features(map, Deidara, Saken, player_id, p_id, type);
            fprintf(stderr, "Player id is %d Type is %d\n", p_id, type);
        }
        fprintf(stderr, "Deidara teleport %d jump %d\n", Deidara->f_t, Deidara->f_j);
        fprintf(stderr, "Saken teleport %d jump %d\n", Saken->f_t, Saken->f_j);

        /*
        Fill goal
        */
        fprintf(stderr, "Before fill_goal\n");
        fill_goal(map, Deidara, h, w, status);
        fprintf(stderr, "After fill_goal\n");
        print_map(map, h, w, 4);

        /*
        fill_enemy_goal
        */
        if (Saken->x != -1)
        {
            fprintf(stderr, "Before fill enemy goal\n");
            fill_enemy_goal(map, Saken, h, w);
            fprintf(stderr, "After fill enemy goal\n");
            print_map(map, h, w, 5);
        }

        /*
        Fill path
        */
        fprintf(stderr, "Before fill path\n");
        fill_path(map, Deidara, h, w);
        fprintf(stderr, "After fill path\n");
        print_map(map, h, w, 2);

        /*
        Fill_features
        */
        fprintf(stderr, "Before fill features\n");
        fill_features(map, Deidara, h, w);
        fprintf(stderr, "After fill features\n");
        print_map(map, h, w, 9);
        
        /*
        Fill tunnel
            and
        Fill enemy path
            and
        Fill enemy tunnel
        */
        if (Saken->x != -1)
        {
            fprintf(stderr, "Before fill tunnel\n");
            fill_tunnel(map, Saken, h, w);
            fprintf(stderr, "After fill tunnel\n");
            print_map(map, h, w, 1);
            fprintf(stderr, "Before fill enemy path\n");
            fill_enemy_path(map, Saken, h, w);
            fprintf(stderr, "After fill enemy path\n");
            print_map(map, h, w, 6);
            fprintf(stderr, "Before fill enemy tunnel\n");
            fill_enemy_tunnel(map, Saken, h, w);
            fprintf(stderr, "After fill enemy tunnel\n");
            print_map(map, h, w, 3);
        }

        /*
        Change goals if dangerous tunnel
        */
        fprintf(stderr, "Before anti_trap\n");
        anti_trap();
        fprintf(stderr, "After anti_trap\n");
        print_map(map, h, w, 10);

        /*
        Final enemy map
        */
        struct pos* enemy_aim = malloc(sizeof(struct pos));
        struct pos* kill_aim = malloc(sizeof(struct pos));
        if (Saken->x != -1)
        {
            enemy_aim->y = Saken->y;
            enemy_aim->x = Saken->x;
            fprintf(stderr, "Before fill final enemy map\n");
            final_enemy_map(map, Saken, h, w, enemy_aim);
            fprintf(stderr, "After fill final enemy map\n");
            print_map(map, h, w, 7);
            adjust_priority(map, Deidara, Saken, status, enemy_aim, h, w);
        }


        /*
        Fill final goal
        */
        int dir = 5;
        struct pos* aim = malloc(sizeof(struct pos));
        aim->y = Deidara->y;
        aim->x = Deidara->x;
        fprintf(stderr, "Min tick %d\n", min_tick);
        fprintf(stderr, "Before final map\n");
        final_map(map, Deidara, Saken, h, w, aim, enemy_aim, &min_tick);
        fprintf(stderr, "After final map\n");
        print_map(map, h, w, 8);

        /*
        Kill bot
        */
        int kill_command = 0;
        if (Saken->x != -1)
        {
            fprintf(stderr, "Before kill_bot\n");
            kill_command = kill_bot(map, Deidara, Saken, kill_aim, enemy_aim, h, w);
            fprintf(stderr, "After kill_bot\n");
        }

        /*
        Fill the direction
        */
        if (kill_command == 0)
        {
            fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
            find_dir(map, Deidara, aim, h, w, &dir);
        }
        else
        {
            fprintf(stderr, "Final aim to kill is %d %d %d Enemy %d %d\n", map[aim->y][aim->x].goal, kill_aim->y, kill_aim->x, enemy_aim->y, enemy_aim->x);
            int failure = target_acqured(map, Deidara, Saken, kill_aim, enemy_aim, h, w, &dir);
            if (failure == 1)
            {
                fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                find_dir(map, Deidara, aim, h, w, &dir);
            }
        }



        struct pos* tp_aim = malloc(sizeof(struct pos)); // TEMPORARY SOLUTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        tp_aim->y = enemy_aim->y;
        tp_aim->x = enemy_aim->x;



        /*
        Bot actions
        */
        const char actions[][10] = { "left", "right", "up", "down", "bomb", "stay", "jump", "tp"};
        if (dir == 7)
        {
            printf("tp %d %d", tp_aim->x, tp_aim->y);
        }
        else
        { 
            printf("%s\n", actions[dir]);
        }
        fflush(stdout);

        /*
        Free
        */
        freentf(map, h, w, aim, enemy_aim, kill_aim, sector_count, tp_aim);
    }
    free(Saken);
    free(Deidara);
    free(status);
    return 0;
}



void fill_map(struct cell** map, char* line, int i, int j, int* box_number, struct situation* status, int* sector_count, int h, int w)
{
    switch (line[j])
    {
    case '.':
        map[i][j].box = 1;
        break;

    case ';':
        map[i][j].box = 10;
        (*box_number)++;
        if (i < h/2 && j < w/2)
        {
            sector_count[0]++;
        }
        if (i < h/2 && j > w/2)
        {
            sector_count[1]++;
        }
        if (i > h/2 && j > w/2)
        {
            sector_count[2]++;
        }
        if (i > h/2 && j < w/2)
        {
            sector_count[3]++;
        }
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
    map[i][j].enemy_delay = 0;
    map[i][j].enemy_path = 1000;
    map[i][j].enemy_goal = 0;
    map[i][j].tunnel = 0;
    map[i][j].enemy_tunnel = 0;
    map[i][j].features = 0;
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
        fprintf(stderr, "Feature amount: x - %d, y - %d\n", x, y);
        break;

    case 'l':
        fill_feature(map, Deidara, ent_type, x, y, coef);
        fprintf(stderr, "Feature radius: x - %d, y - %d\n", x, y);
        break;

    case 't':
        fill_feature(map, Deidara, ent_type, x, y, coef);
        fprintf(stderr, "Feature teleport: x - %d, y - %d\n", x, y);
        break;

    case 'j':
        fill_feature(map, Deidara, ent_type, x, y, coef);
        fprintf(stderr, "Feature jump: x - %d, y - %d\n", x, y);
        break;

    default: break;
    }
}

void fill_bomb(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int* min_tick)
{
    int less_than = 0;
    if (player_id == p_id)
        less_than = 5 - Deidara->f_r;
    else
        less_than = 5 - Saken->f_r;

    if (player_id == p_id && Deidara->f_a == 0)
    {
        if (*min_tick == 0 || *min_tick > param_1)
        {
            *min_tick = param_1;
        }
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
    map[y][x].features = -1;
    if (Deidara->f_a > Deidara->a_a)
        Deidara->a_a = Deidara->f_a;
    //fprintf(stderr, "COEFF and a_a %d %d \n", 8 * coef / 100, Deidara->a_a);
    if (ent_type == 'a')
    {
        if (Deidara->a_a == 1)
            map[y][x].goal += 10 * coef / 100;
    }
    else if (ent_type == 'l')
    {
        if (Deidara->f_r == 2 || Deidara->f_r == 3)
            map[y][x].goal += 10 * coef / 100;
    }
    else if (ent_type == 't')
    {
        if (Deidara->f_t == 0)
            map[y][x].goal += 30 * coef / 100;
    }
    else if (ent_type == 'j')
    {
        if (Deidara->f_j == 0)
            map[y][x].goal += 20 * coef / 100;
    }
}



void read_features(struct cell** map, struct bot* Deidara, struct bot* Saken, int player_id, int p_id, int type)
{
    if (p_id == player_id)
    {
        if (type == 0)
        {
            Deidara->f_j = 1;
        }
        else if (type == 1)
        {
            Deidara->f_t = 1;
        }
    }
    else
    {
        if (type == 0)
        {
            Saken->f_j = 1;
        }
        else if (type == 1)
        {
            Saken->f_t = 1;
        }
    }
}



void fill_goal(struct cell** map, struct bot* Deidara, int h, int w, struct situation* status)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].box == 10)
                fill_box(map, Deidara, i, j, h, w, status);
        }
    }
}

void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w, struct situation* status)
{
    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal += value;

            quadrant_priority(map, status, y, tmp_x, h, w);
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

            quadrant_priority(map, status, y, tmp_x, h, w);
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

            quadrant_priority(map, status, tmp_y, x, h, w);
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

            quadrant_priority(map, status, tmp_y, x, h, w);
        }
        else
            break;
    }
}

void quadrant_priority(struct cell** map, struct situation* status, int i, int j, int h, int w)
{
    int added_priority = 1;
    if (status->half_game == 1 && map[i][j].goal == value)
    {
        switch (status->p_sector)
        {
        case 0:
            if (i < h / 2 && j < w / 2)
            {
                map[i][j].goal += added_priority;
            }
            break;
        case 1:
            if (i < h / 2 && j > w / 2)
            {
                map[i][j].goal += added_priority;
            }
            break;
        case 2:
            if (i > h / 2 && j > w / 2)
            {
                map[i][j].goal += added_priority;
            }
            break;
        case 3:
            if (i > h / 2 && j < w / 2)
            {
                map[i][j].goal += added_priority;
            }
            break;
        default:
            break;
        }
    }
}


void fill_enemy_goal(struct cell** map, struct bot* Saken, int h, int w)
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
    get_point(map, y, x, step, prev, h, w, bomb);
}

void get_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb)
{
    int box_around = 0;
    if (map[y][x].box <= 1 && map[y][x].tunnel == 0)
    {
        if (y - 1 < 0) box_around++;
        else if (map[y - 1][x].box > 1) box_around++;
        if (y + 1 >= h) box_around++;
        else if (map[y + 1][x].box > 1) box_around++;
        if (x - 1 < 0) box_around++;
        else if (map[y][x - 1].box > 1) box_around++;
        if (x + 1 >= w) box_around++;
        else if (map[y][x + 1].box > 1) box_around++;
        if (box_around == 3)
        {
            map[y][x].tunnel = -1;
            // fprintf(stderr, "Added %d %d.\n", y, x);
        }
    }

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
        get_point(map, y, x + 1, step, 'R', h, w, bomb);
    if (prev != 'U' && y + 1 < h)
        get_point(map, y + 1, x, step, 'D', h, w, bomb);
    if (prev != 'R' && x - 1 >= 0)
        get_point(map, y, x - 1, step, 'L', h, w, bomb);
    if (prev != 'D' && y - 1 >= 0)
        get_point(map, y - 1, x, step, 'U', h, w, bomb);
}



void fill_features(struct cell** map, struct bot* Deidara, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Deidara->x;
    int y = Deidara->y;
    int features = 0;
    get_features(map, y, x, step, prev, h, w, features);
}

void get_features(struct cell** map, int y, int x, int step, int prev, int h, int w, int features)
{
    if (step == 0)
    {
        step++;
    }
    else if (map[y][x].box <= 1 && step == map[y][x].path)
    {
        if (map[y][x].features < 0)
        {
            features -= map[y][x].features;
            map[y][x].features = -features;
        }
        else if (features > map[y][x].features)
        {
            map[y][x].features = features;
        }
        step++;
    }
    else
        return;

    if (prev != 'L' && x + 1 < w)
        get_features(map, y, x + 1, step, 'R', h, w, features);
    if (prev != 'U' && y + 1 < h)
        get_features(map, y + 1, x, step, 'D', h, w, features);
    if (prev != 'R' && x - 1 >= 0)
        get_features(map, y, x - 1, step, 'L', h, w, features);
    if (prev != 'D' && y - 1 >= 0)
        get_features(map, y - 1, x, step, 'U', h, w, features);
}



void fill_tunnel(struct cell** map, struct bot* Saken, int h, int w)
{
    if (Saken->x == -1) return;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (map[y][x].tunnel == -1)
            {
                if (y - 1 >= 0)
                    if (map[y - 1][x].path != 1000)
                    {
                        get_tunnels(map, Saken, y - 1, x, 1, h, w);
                    }
                if (y + 1 < h)
                    if (map[y + 1][x].path != 1000)
                    {
                        get_tunnels(map, Saken, y + 1, x, 1, h, w);
                    }
                if (x - 1 >= 0)
                    if (map[y][x - 1].path != 1000)
                    {
                        get_tunnels(map, Saken, y, x - 1, 1, h, w);
                    }
                if (x + 1 < w)
                    if (map[y][x + 1].path != 1000)
                    {
                        get_tunnels(map, Saken, y, x + 1, 1, h, w);
                    }
            }
        }
    }
}

void get_tunnels(struct cell** map, struct bot* Saken, int y, int x, int deep, int h, int w)
{
    int box_count = 0;
    if (y - 1 < 0) box_count++;
    else if (map[y - 1][x].box > 1 && map[y - 1][x].box != 50) box_count++;
    if (y + 1 >= h) box_count++;
    else if (map[y + 1][x].box > 1 && map[y + 1][x].box != 50) box_count++;
    if (x - 1 < 0) box_count++;
    else if (map[y][x - 1].box > 1 && map[y][x - 1].box != 50) box_count++;
    if (x + 1 >= w) box_count++;
    else if (map[y][x + 1].box > 1 && map[y][x + 1].box != 50) box_count++;
    if (box_count == 2)
    {
        map[y][x].tunnel = deep + 1;
        if (map[y][x].tunnel <= Saken->f_r + 1) map[y][x].tunnel *= -1;
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].path != 1000 && (map[y - 1][x].tunnel == 0 || map[y - 1][x].tunnel > 100)) get_tunnels(map, Saken, y - 1, x, deep + 1, h, w);
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].path != 1000 && (map[y + 1][x].tunnel == 0 || map[y + 1][x].tunnel > 100)) get_tunnels(map, Saken, y + 1, x, deep + 1, h, w);
        }
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].path != 1000 && (map[y][x - 1].tunnel == 0 || map[y][x - 1].tunnel > 100)) get_tunnels(map, Saken, y, x - 1, deep + 1, h, w);
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].path != 1000 && (map[y][x + 1].tunnel == 0 || map[y][x + 1].tunnel > 100)) get_tunnels(map, Saken, y, x + 1, deep + 1, h, w);
        }
    }
    else
    {
        if (map[y][x].tunnel > 100)
        {
            if (map[y][x].tunnel <= 100 + deep + 1) map[y][x].tunnel = 200 + deep + 1;
            else map[y][x].tunnel += 100;
        }
        else map[y][x].tunnel = 100 + deep + 1;
    }
}



void fill_enemy_path(struct cell** map, struct bot* Saken, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Saken->x;
    int y = Saken->y;
    int bomb = 0;
    get_enemy_point(map, y, x, step, prev, h, w, bomb);
}

void get_enemy_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb)
{
    int box_around = 0;
    if (map[y][x].box <= 1 && map[y][x].enemy_tunnel == 0)
    {
        if (y - 1 < 0) box_around++;
        else if (map[y - 1][x].box > 1) box_around++;
        if (y + 1 >= h) box_around++;
        else if (map[y + 1][x].box > 1) box_around++;
        if (x - 1 < 0) box_around++;
        else if (map[y][x - 1].box > 1) box_around++;
        if (x + 1 >= w) box_around++;
        else if (map[y][x + 1].box > 1) box_around++;
        if (box_around == 3)
        {
            map[y][x].enemy_tunnel = -1;
            // fprintf(stderr, "Added %d %d.\n", y, x);
        }
    }
    if (step == 0)
    {
        map[y][x].enemy_path = step++;
        map[y][x].enemy_delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box == 1 && step < map[y][x].enemy_path)
    {
        map[y][x].enemy_path = step++;
        map[y][x].enemy_delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box < 0 && step < map[y][x].enemy_path)
    {
        bomb++;
        if (map[y][x].box != -1 * step)
        {
            map[y][x].enemy_path = step++;
            map[y][x].enemy_delay = 0;
        }
        else
        {
            step += bomb;
            map[y][x].enemy_path = step++;
            map[y][x].enemy_delay = bomb;
        }
    }
    else
        return;

    if (prev != 'L' && x + 1 < w)
        get_enemy_point(map, y, x + 1, step, 'R', h, w, bomb);
    if (prev != 'U' && y + 1 < h)
        get_enemy_point(map, y + 1, x, step, 'D', h, w, bomb);
    if (prev != 'R' && x - 1 >= 0)
        get_enemy_point(map, y, x - 1, step, 'L', h, w, bomb);
    if (prev != 'D' && y - 1 >= 0)
        get_enemy_point(map, y - 1, x, step, 'U', h, w, bomb);
}



void fill_enemy_tunnel(struct cell** map, struct bot* Saken, int h, int w)
{
    if (Saken->x == -1) return;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (map[y][x].enemy_tunnel == -1)
            {
                if (y - 1 >= 0)
                    if (map[y - 1][x].enemy_path != 1000)
                    {
                        get_enemy_tunnels(map, Saken, y - 1, x, 1, h, w);
                    }
                if (y + 1 < h)
                    if (map[y + 1][x].enemy_path != 1000)
                    {
                        get_enemy_tunnels(map, Saken, y + 1, x, 1, h, w);
                    }
                if (x - 1 >= 0)
                    if (map[y][x - 1].enemy_path != 1000)
                    {
                        get_enemy_tunnels(map, Saken, y, x - 1, 1, h, w);
                    }
                if (x + 1 < w)
                    if (map[y][x + 1].enemy_path != 1000)
                    {
                        get_enemy_tunnels(map, Saken, y, x + 1, 1, h, w);
                    }
            }
        }
    }
}

void get_enemy_tunnels(struct cell** map, struct bot* Saken, int y, int x, int deep, int h, int w)
{
    int box_count = 0;
    if (y - 1 < 0) box_count++;
    else if (map[y - 1][x].box > 1 && map[y - 1][x].box != 50) box_count++;
    if (y + 1 >= h) box_count++;
    else if (map[y + 1][x].box > 1 && map[y + 1][x].box != 50) box_count++;
    if (x - 1 < 0) box_count++;
    else if (map[y][x - 1].box > 1 && map[y][x - 1].box != 50) box_count++;
    if (x + 1 >= w) box_count++;
    else if (map[y][x + 1].box > 1 && map[y][x + 1].box != 50) box_count++;
    if (box_count == 2)
    {
        map[y][x].enemy_tunnel = deep + 1;
        if (map[y][x].enemy_tunnel <= Saken->f_r + 1) map[y][x].enemy_tunnel *= -1;
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].enemy_path != 1000 && (map[y - 1][x].enemy_tunnel == 0 || map[y - 1][x].enemy_tunnel > 100)) get_enemy_tunnels(map, Saken, y - 1, x, deep + 1, h, w);
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].enemy_path != 1000 && (map[y + 1][x].enemy_tunnel == 0 || map[y + 1][x].enemy_tunnel > 100)) get_enemy_tunnels(map, Saken, y + 1, x, deep + 1, h, w);
        }
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].enemy_path != 1000 && (map[y][x - 1].enemy_tunnel == 0 || map[y][x - 1].enemy_tunnel > 100)) get_enemy_tunnels(map, Saken, y, x - 1, deep + 1, h, w);
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].enemy_path != 1000 && (map[y][x + 1].enemy_tunnel == 0 || map[y][x + 1].enemy_tunnel > 100)) get_enemy_tunnels(map, Saken, y, x + 1, deep + 1, h, w);
        }
    }
    else
    {
        if (map[y][x].enemy_tunnel > 100)
        {
            if (map[y][x].enemy_tunnel <= 100 + deep + 1) map[y][x].enemy_tunnel = 200 + deep + 1;
            else map[y][x].enemy_tunnel += 100;
        }
        else map[y][x].enemy_tunnel = 100 + deep + 1;
    }
}



void anti_trap()
{
    int trap_pos = 0;
    struct pos* tmp_aim = malloc(sizeof(struct pos));
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].tunnel < 0)
            {

            }
        }
    }
    free(tmp_aim);
}

int find_anti_trap(struct cell** map, struct pos* tmp_aim, int y, int x, int fin_value, int h, int w, int* anti_timeout)
{
    (*anti_timeout)++; // temporary solution!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (*anti_timeout == 100) return 0;
    if (y - 1 >= 0)
    {
        if (my_abs(map[y - 1][x].tunnel) == fin_value || my_abs(map[y - 1][x].tunnel) > 100)
        {
            tmp_aim->y = y - 1;
            tmp_aim->x = x;
            return 1;
        }
        else if (my_abs(map[y - 1][x].tunnel) == my_abs(map[y][x].tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y - 1, x, fin_value, h, w, anti_timeout);
        }
    }
    if (y + 1 < h)
    {
        if (my_abs(map[y + 1][x].tunnel) == fin_value || my_abs(map[y + 1][x].tunnel) > 100)
        {
            tmp_aim->y = y + 1;
            tmp_aim->x = x;
            return 1;
        }
        else if (my_abs(map[y + 1][x].tunnel) == my_abs(map[y][x].tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y + 1, x, fin_value, h, w, anti_timeout);
        }
    }
    if (x - 1 >= 0)
    {
        if (my_abs(map[y][x - 1].tunnel) == fin_value || my_abs(map[y][x - 1].tunnel) > 100)
        {
            tmp_aim->y = y;
            tmp_aim->x = x - 1;
            return 1;
        }
        else if (my_abs(map[y][x - 1].tunnel) == my_abs(map[y][x].tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y, x - 1, fin_value, h, w, anti_timeout);
        }
    }
    if (x + 1 < w)
    {
        if (my_abs(map[y][x + 1].tunnel) == fin_value || my_abs(map[y][x + 1].tunnel) > 100)
        {
            tmp_aim->y = y;
            tmp_aim->x = x + 1;
            return 1;
        }
        else if (my_abs(map[y][x + 1].tunnel) == my_abs(map[y][x].tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y, x + 1, fin_value, h, w, anti_timeout);
        }
    }
    return 1;
}



void final_enemy_map(struct cell** map, struct bot* Saken, int h, int w, struct pos* enemy_aim)
{
    if (map[Saken->y][Saken->x].enemy_goal == 0)
        map[Saken->y][Saken->x].enemy_goal = -135;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].enemy_goal == 0)
                map[i][j].enemy_goal = -135;
            map[i][j].enemy_goal -= map[i][j].enemy_path;
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



void adjust_priority(struct cell** map, struct bot* Deidara, struct bot* Saken, struct situation* status, struct pos* enemy_aim, int h, int w)
{
    int y = enemy_aim->y;
    int x = enemy_aim->x;
    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (map[y][tmp_x].box == 10)
        {
            adjust_box(map, Deidara, enemy_aim, y, tmp_x, h, w, status);
            break;
        }
    }

    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;

        if (map[y][tmp_x].box == 10)
        {
            adjust_box(map, Deidara, enemy_aim, y, tmp_x, h, w, status);
            break;
        }
    }

    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;

        if (map[tmp_y][x].box == 10)
        {
            adjust_box(map, Deidara, enemy_aim, tmp_y, x, h, w, status);
            break;
        }
    }

    for (int i = 1; i <= Saken->f_r; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;

        if (map[tmp_y][x].box == 10)
        {
            adjust_box(map, Deidara, enemy_aim, tmp_y, x, h, w, status);
            break;
        }
    }
}

void adjust_box(struct cell** map, struct bot* Deidara, struct pos* enemy_aim, int y, int x, int h, int w, struct situation* status)
{
    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (adjust_one_way(map, status, enemy_aim, y, tmp_x, h, w) == 1)
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;

        if (adjust_one_way(map, status, enemy_aim, y, tmp_x, h, w) == 1)
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;

        if (adjust_one_way(map, status, enemy_aim, tmp_y, x, h, w) == 1)
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;

        if (adjust_one_way(map, status, enemy_aim, tmp_y, x, h, w) == 1)
            break;
    }
}

int adjust_one_way(struct cell** map, struct situation* status, struct pos* enemy_aim, int y, int x, int h, int w)
{
    int good_value = 3;
    int equal_value = 1;
    if (map[y][x].box <= 1 && map[y][x].path < map[enemy_aim->y][enemy_aim->x].enemy_path)
    {
        map[y][x].goal += good_value;
    }
    else if (map[y][x].box <= 1 && map[y][x].path == map[enemy_aim->y][enemy_aim->x].enemy_path)
    {
        map[y][x].goal += equal_value;
    }
    else if (map[y][x].box <= 1 && map[y][x].path > map[enemy_aim->y][enemy_aim->x].enemy_path)
    {
        map[y][x].goal -= value;
        adjust_quadrant(map, status, y, x, h, w);
    }
    else
        return 1;
    return 0;
}

void adjust_quadrant(struct cell** map, struct situation* status, int y, int x, int h, int w)
{
    int added_priority = 1;
    if (status->half_game == 1 && map[y][x].goal % value == 1)
    {
        switch (status->p_sector)
        {
        case 0:
            if (y < h / 2 && x < w / 2)
            {
                map[y][x].goal -= added_priority;
            }
            break;
        case 1:
            if (y < h / 2 && x > w / 2)
            {
                map[y][x].goal -= added_priority;
            }
            break;
        case 2:
            if (y > h / 2 && x > w / 2)
            {
                map[y][x].goal -= added_priority;
            }
            break;
        case 3:
            if (y > h / 2 && x < w / 2)
            {
                map[y][x].goal -= added_priority;
            }
            break;
        default:
            break;
        }
    }
}



void final_map(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct pos* aim, struct pos* enemy_aim, int* min_tick)
{
    if (map[Deidara->y][Deidara->x].goal == 0)
        map[Deidara->y][Deidara->x].goal = -135;

    if (Deidara->f_a != 0 && *min_tick != 0)
    {
        *min_tick = 0;
    }

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].goal == 0)
                map[i][j].goal = -135;
            if (*min_tick != 0)
            {
                if (*min_tick <= map[i][j].path)
                    map[i][j].goal -= (map[i][j].path - *min_tick);
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



int kill_bot(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* kill_aim, struct pos* enemy_aim, int h, int w)
{
    if (map[enemy_aim->y][enemy_aim->x].enemy_tunnel < 0)
    {
        struct pos* tmp_aim = malloc(sizeof(struct pos));
        int anti_timeout = 1;
        tmp_aim->y = -1;
        tmp_aim->x = -1;
        fprintf(stderr, "Kill bot: Before find trap pos\n");
        if (find_trap_pos(map, tmp_aim, enemy_aim->y, enemy_aim->x, my_abs(map[enemy_aim->y][enemy_aim->x].enemy_tunnel) + Saken->f_r + 1, h, w, &anti_timeout) == 0)
        {
            fprintf(stderr, "error in find_trap_pos\n");
            free(tmp_aim);
            return 0;
        }
        fprintf(stderr, "Kill bot: After find trap pos\n");
        fprintf(stderr, "tmp_aim %d %d enemy_aim %d %d\n", tmp_aim->y, tmp_aim->x, enemy_aim->y, enemy_aim->x);
        if (tmp_aim->y != -1 && map[tmp_aim->y][tmp_aim->x].path <= map[enemy_aim->y][enemy_aim->x].enemy_path)
        {
            int extra_steps = map[enemy_aim->y][enemy_aim->x].enemy_path - map[tmp_aim->y][tmp_aim->x].path;
            if (extra_steps > 0)
            {
                int anti_timeout_2 = 1;
                struct pos* tmp_aim2 = malloc(sizeof(struct pos));
                tmp_aim2->y = -1;
                tmp_aim2->x = -1;
                if (find_trap_pos(map, tmp_aim2, enemy_aim->y, enemy_aim->x, my_abs(map[enemy_aim->y][enemy_aim->x].enemy_tunnel) + Deidara->f_r, h, w, &anti_timeout_2) == 0)
                {
                    fprintf(stderr, "error in find_trap_pos after extra steps\n");
                    free(tmp_aim2);
                    return 0;
                }
                if (tmp_aim2->y != -1 && map[tmp_aim2->y][tmp_aim2->x].path <= map[enemy_aim->y][enemy_aim->x].enemy_path)
                {
                    tmp_aim->y = tmp_aim2->y;
                    tmp_aim->x = tmp_aim2->x;
                    fprintf(stderr, "Found a case to kill for possibly 7 points and we have extra %d steps.\n", extra_steps);
                }
                free(tmp_aim2);
            }
            fprintf(stderr, "Found a case to trap and we have extra %d steps.\n", extra_steps);
            
            int extra = 1;

            if (kill_aim->y == enemy_aim->y && kill_aim->x == enemy_aim->x)
            {
                fprintf(stderr, "First if in kill bot\n");
                kill_aim->y = tmp_aim->y;
                kill_aim->x = tmp_aim->x;
                free(tmp_aim);
                return 1;
            }
            else if (map[enemy_aim->y][enemy_aim->x].enemy_path - map[tmp_aim->y][tmp_aim->x].path <= extra)
            {
                fprintf(stderr, "Else if in kill bot 1\n");
                kill_aim->y = tmp_aim->y;
                kill_aim->x = tmp_aim->x;
                free(tmp_aim);
                return 1;
            }
            else if (map[enemy_aim->y][enemy_aim->x].enemy_path - map[tmp_aim->y][tmp_aim->x].path == -1)
            {
                fprintf(stderr, "Else if in kill bot 2\n");
                if (my_abs(enemy_aim->y - tmp_aim->y) + my_abs(enemy_aim->x - tmp_aim->x) > 1)
                {
                    kill_aim->y = tmp_aim->y;
                    kill_aim->x = tmp_aim->x;
                    free(tmp_aim);
                    return 1;
                }
            }
        }
        free(tmp_aim);
    }
    return 0;
}

int find_trap_pos(struct cell** map, struct pos* tmp_aim, int y, int x, int fin_value, int h, int w, int* anti_timeout)
{
    (*anti_timeout)++; // temporary solution!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (*anti_timeout == 100) return 0;
    if (y - 1 >= 0)
    {
        if (my_abs(map[y - 1][x].enemy_tunnel) == fin_value || my_abs(map[y - 1][x].enemy_tunnel) > 100)
        {
            tmp_aim->y = y - 1;
            tmp_aim->x = x;
            return 1;
        }
        else if (my_abs(map[y - 1][x].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y - 1, x, fin_value, h, w, anti_timeout);
        }
    }
    if (y + 1 < h)
    {
        if (my_abs(map[y + 1][x].enemy_tunnel) == fin_value || my_abs(map[y + 1][x].enemy_tunnel) > 100)
        {
            tmp_aim->y = y + 1;
            tmp_aim->x = x;
            return 1;
        }
        else if (my_abs(map[y + 1][x].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y + 1, x, fin_value, h, w, anti_timeout);
        }
    }
    if (x - 1 >= 0)
    {
        if (my_abs(map[y][x - 1].enemy_tunnel) == fin_value || my_abs(map[y][x - 1].enemy_tunnel) > 100)
        {
            tmp_aim->y = y;
            tmp_aim->x = x - 1;
            return 1;
        }
        else if (my_abs(map[y][x - 1].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y, x - 1, fin_value, h, w, anti_timeout);
        }
    }
    if (x + 1 < w)
    {
        if (my_abs(map[y][x + 1].enemy_tunnel) == fin_value || my_abs(map[y][x + 1].enemy_tunnel) > 100)
        {
            tmp_aim->y = y;
            tmp_aim->x = x + 1;
            return 1;
        }
        else if (my_abs(map[y][x + 1].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1)
        {
            find_trap_pos(map, tmp_aim, y, x + 1, fin_value, h, w, anti_timeout);
        }
    }
    return 1;
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
        int current_priority = 0;
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].path == map[y][x].path - 1 - map[y][x].delay && ((map[y][x - 1].features < 0 && map[y][x - 1].features < current_priority) || (current_priority >= 0 && map[y][x - 1].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 1;
                current_priority = map[y][x - 1].features;
                //x--;
                //continue;
            }
        }
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].path == map[y][x].path - 1 - map[y][x].delay && ((map[y - 1][x].features < 0 && map[y - 1][x].features < current_priority) || (current_priority >= 0 && map[y - 1][x].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 3;
                current_priority = map[y - 1][x].features;
                //y--;
                //continue;
            }
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].path == map[y][x].path - 1 - map[y][x].delay && ((map[y][x + 1].features < 0 && map[y][x + 1].features < current_priority) || (current_priority >= 0 && map[y][x + 1].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 0;
                current_priority = map[y][x + 1].features;
                //x++;
                //continue;
            }
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].path == map[y][x].path - 1 - map[y][x].delay && ((map[y + 1][x].features < 0 && map[y + 1][x].features < current_priority) || (current_priority >= 0 && map[y + 1][x].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 2;
                current_priority = map[y + 1][x].features;
                //y++;
                //continue;
            }
        }
        if (*dir == 1) x--;
        if (*dir == 3) y--;
        if (*dir == 0) x++;
        if (*dir == 2) y++;
        fprintf(stderr, "Dir is %d\n", *dir);
        if (anti_timeout < 50) anti_timeout++;
        else if (anti_timeout >= 50)
        {
            *dir = 5;

            fprintf(stderr, "ALERT ALERT ALERT ALERT Find_dir timeout.\n");
            /*if (x - 1 >= 0) //ACTIVATE FOR ARENA
                if (map[y][x - 1].box <= 1)
                    *dir = 0;
            if (y - 1 >= 0)
                if (map[y - 1][x].box <= 1)
                    *dir = 2;
            if (x + 1 < w)
                if (map[y][x + 1].box <= 1)
                    *dir = 1;
            if (y + 1 < h)
                if (map[y + 1][x].box <= 1)
                    *dir = 3;*/
            break;
        }
    }

    if (wait != 0)
    {
        fprintf(stderr, "escape?\n");
        fprintf(stderr, "wait %d dir %d next %d %d %d\n", wait, *dir, next.y, next.x, map[next.y][next.x].box);
        if ((map[Deidara->y][Deidara->x].box == -1 || map[Deidara->y][Deidara->x].box == 50) && map[next.y][next.x].box == -1)
        {
            fprintf(stderr, "escape first if\n");
            if (get_out(map, Deidara, dir, h, w, -1) == 1)
            {
                if (Deidara->f_j == 1)
                {
                    *dir = 6;
                }
                else if (Deidara->f_t == 1)
                {
                    *dir = 7;
                }
            }
            fprintf(stderr, "new dir is %d\n", *dir);
        }
        else if (map[Deidara->y][Deidara->x].box == -2 && map[next.y][next.x].box == -2)
        {
            fprintf(stderr, "escape second if\n");
            get_out(map, Deidara, dir, h, w, -2);
            fprintf(stderr, "new dir is %d\n", *dir);
        }
        else if ((map[Deidara->y][Deidara->x].box >= 1 || map[Deidara->y][Deidara->x].box < -1) && wait == -1 * map[next.y][next.x].box)
        {
            *dir = 5;
        }
    }
}

int target_acqured(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* aim, struct pos* enemy_aim, int h, int w, int* dir)
{
    int anti_timeout = 0;
    int y = aim->y;
    int x = aim->x;
    int wait = 0;
    struct pos next = { -1, -1 };

    if (map[aim->y][aim->x].box != 1)
    {
        return 1;
    }

    if (Deidara->y == aim->y && Deidara->x == aim->x)
    {
        if (map[Saken->y][Saken->x].enemy_tunnel < 0 && my_abs(map[aim->y][aim->x].enemy_tunnel) > my_abs(map[Saken->y][Saken->x].enemy_tunnel))
        {
            *dir = 4;
        }
        else
        {
            *dir = 5;
        }
        return 0;
    }

    while (y != Deidara->y || x != Deidara->x)
    {
        int current_priority = 0;
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].path == map[y][x].path - 1 - map[y][x].delay && ((map[y][x - 1].features < 0 && map[y][x - 1].features < current_priority) || (current_priority >= 0 && map[y][x - 1].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 1;
                current_priority = map[y][x - 1].features;
                //x--;
                //continue;
            }
        }
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].path == map[y][x].path - 1 - map[y][x].delay && ((map[y - 1][x].features < 0 && map[y - 1][x].features < current_priority) || (current_priority >= 0 && map[y - 1][x].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 3;
                current_priority = map[y - 1][x].features;
                //y--;
                //continue;
            }
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].path == map[y][x].path - 1 - map[y][x].delay && ((map[y][x + 1].features < 0 && map[y][x + 1].features < current_priority) || (current_priority >= 0 && map[y][x + 1].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 0;
                current_priority = map[y][x + 1].features;
                //x++;
                //continue;
            }
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].path == map[y][x].path - 1 - map[y][x].delay && ((map[y + 1][x].features < 0 && map[y + 1][x].features < current_priority) || (current_priority >= 0 && map[y + 1][x].features >= current_priority)))
            {
                next.y = y;
                next.x = x;
                if (map[y][x].delay != 0)
                {
                    wait = map[y][x].delay;
                }
                *dir = 2;
                current_priority = map[y + 1][x].features;
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

    if (wait != 0)
    {
        fprintf(stderr, "escape?\n");
        fprintf(stderr, "wait %d dir %d next %d %d %d\n", wait, *dir, next.y, next.x, map[next.y][next.x].box);
        if ((map[Deidara->y][Deidara->x].box == -1 || map[Deidara->y][Deidara->x].box == 50) && map[next.y][next.x].box == -1)
        {
            fprintf(stderr, "escape first if\n");
            if (get_out(map, Deidara, dir, h, w, -1) == 1)
            {
                if (Deidara->f_j == 1)
                {
                    *dir = 6;
                }
                else if (Deidara->f_t == 1)
                {
                    *dir = 7;
                }
            }
            fprintf(stderr, "new dir is %d\n", *dir);
        }
        else if (map[Deidara->y][Deidara->x].box == -2 && map[next.y][next.x].box == -2)
        {
            fprintf(stderr, "escape second if\n");
            get_out(map, Deidara, dir, h, w, -2);
            fprintf(stderr, "new dir is %d\n", *dir);
        }
        else if ((map[Deidara->y][Deidara->x].box >= 1 || map[Deidara->y][Deidara->x].box < -1) && wait == -1 * map[next.y][next.x].box)
        {
            *dir = 5;
        }
    }
    return 0;
}

int get_out(struct cell** map, struct bot* Deidara, int* dir, int h, int w, int tick)
{
    int x = Deidara->x;
    int y = Deidara->y;
    if (x - 1 >= 0)
    {
        if (map[y][x - 1].box == 1 || map[y][x - 1].box < tick)
        {
            *dir = 0;
            return 0;
        }
    }
    if (x + 1 < w)
    {
        if (map[y][x + 1].box == 1 || map[y][x + 1].box < tick)
        {
            *dir = 1;
            return 0;
        }
    }
    if (y - 1 >= 0)
    {
        if (map[y - 1][x].box == 1 || map[y - 1][x].box < tick)
        {
            *dir = 2;
            return 0;
        }
    }
    if (y + 1 < h)
    {
        if (map[y + 1][x].box == 1 || map[y + 1][x].box < tick)
        {
            *dir = 3;
            return 0;
        }
    }
    return 1;
}



void print_map(struct cell** map, int h, int w, int map_index)
{
    switch (map_index)
    {
        case 0:
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
            break;
        case 1:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "tunnel: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%4d ", map[i][j].tunnel);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        case 2:
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
            break;
        case 3:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "Enemy tunnel: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%4d ", map[i][j].enemy_tunnel);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        case 4:
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
            break;
        case 5:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "enemy goal: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%5d ", map[i][j].enemy_goal);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        case 6:
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
            break;
        case 7:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "Final enemy goal: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%5d ", map[i][j].enemy_goal);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        case 8:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "Final goal: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%5d ", map[i][j].goal);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        case 9:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "Features: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%5d ", map[i][j].features);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        case 10:
            fprintf(stderr, "\n");
            for (int i = 0; i < h; i++)
            {
                fprintf(stderr, "goal with anti_trap: ");
                for (int j = 0; j < w; j++)
                {
                    fprintf(stderr, "%5d ", map[i][j].goal);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            break;
        default:
            break;
    }
}

void freentf(struct cell** map, int h, int w, struct pos* aim, struct pos* enemy_aim, struct pos* kill_aim, int* sector_count, struct pos* tp_aim)
{
    //free part
    for (int i = 0; i < w; i++)
    {
        free(map[i]);
    }
    free(enemy_aim);
    free(aim);
    free(kill_aim);
    free(map);
    free(sector_count);
    free(tp_aim);
}

int my_abs_dif(int n1, int n2)
{
    int res = n1 - n2;
    if (res < 0) return -res;
    return res;
}

int my_abs(int n)
{
    if (n < 0) return -n;
    return n;
}