#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define value 4
#define enemy_value 4
#define feature_value 10
#define quadrant_value 2

struct cell
{
    int box, path, goal, delay, tunnel, enemy_tunnel, enemy_delay, enemy_path, enemy_goal, features, bomb_radius, bomb_tick, wood_box, extragoal, bomb_Saken, bomb_Deidara;
    int enemy_box_2, enemy_goal_2, enemy_path_2, enemy_tunnel_2;
    int surround;
    int adjust;
};

struct bot
{
    int x, y, f_a, f_r, a_a, f_t, f_j, box_count, future_box_count;
};

struct pos
{
    int x, y;
};

struct situation
{
    int assassin, assassin_wait, half_game, p_sector, teleport, tp_on_map, tunnel_kill, tunnel_wait, out_of_p_sector;
    int farm, go_farm;
    int super, potential, ART, explode;
};

void fill_map(struct cell** map, char* line, int i, int j, int* box_number, struct situation* status, int* sector_count, int h, int w);

void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w, int coef, int* min_tick, struct situation* status, struct pos* tp_on_map_pos);
void fill_bomb(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int* min_tick);
int fill_bomb_dir(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int less_than, int* min_tick);
void fill_feature(struct cell** map, struct bot* Deidara, struct bot* Saken, struct situation* status, char ent_type, int x, int y, int coef, int h, int w);
void future_sector_count(struct cell** map, int* future_number, int* sector_count, int h, int w);
void out_of_p_sector(struct cell** map, struct bot* Deidara, struct situation* status, int h, int w);

void adjust_bomb_goal(struct cell** map, struct bot* Deidara, int h, int w);

void fill_bomb_Saken(struct cell** map, int player_id, int h, int w);
void get_bomb_Saken(struct cell** map, int x, int y, int h, int w);
void fill_bomb_Deidara(struct cell** map, int player_id, int h, int w);
void get_bomb_Deidara(struct cell** map, int x, int y, int h, int w);

void count_box_points(struct cell** map, struct bot* Deidara, struct bot* Saken, int player_id, int h, int w);
void get_box_points(struct cell** map, struct bot* bot, int x, int y, int h, int w);
void get_future_box_points(struct cell** map, struct bot* bot, int j, int i, int h, int w, int* additional_box_count);

void read_features(struct cell** map, struct bot* Deidara, struct bot* Saken, int player_id, int p_id, int type);

void fill_goal(struct cell** map, struct bot* Deidara, int h, int w, struct situation* status);
void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w, struct situation* status, int goal_val);
void quadrant_priority(struct cell** map, struct situation* status, int i, int j, int h, int w);

void fill_enemy_goal(struct cell** map, struct bot* Saken, int h, int w);
void fill_enemy_box(struct cell** map, struct bot* Saken, int y, int x, int h, int w);

void fill_path(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w);
void get_point(struct cell** map, struct bot* Deidara, struct bot* Saken, int y, int x, int step, int prev, int h, int w, int bomb);
int Saken_nearby(struct cell** map, struct bot* Saken, int y, int x, int h, int w);

void add_feature_goal(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct situation* status, int coef);
void add_feat_goal(struct cell** map, struct bot* Deidara, struct bot* Saken, int i, int j, int h, int w, struct situation* status, int feat_value, int coef);

void fill_extragoal(struct cell** map, struct bot* Deidara, int player_id, int h, int w);
void get_extragoal(struct cell** map, struct bot* Deidara, int player_id, int x, int y, int h, int w);
void get_extragoal_part_2(struct cell** map, struct bot* Deidara, int player_id, int x, int y, int h, int w);

void fill_features(struct cell** map, struct bot* Deidara, int h, int w);
void get_features(struct cell** map, int y, int x, int step, int prev, int h, int w, int features);

void fill_tunnel(struct cell** map, struct bot* Saken, int h, int w);
void get_tunnels(struct cell** map, struct bot* Saken, int y, int x, int deep, int h, int w);

void fill_enemy_path(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w);
void get_enemy_point(struct cell** map, struct bot* Deidara, struct bot* Saken, int y, int x, int step, int prev, int h, int w, int bomb);

void fill_enemy_tunnel(struct cell** map, struct bot* Saken, int h, int w);
void get_enemy_tunnels(struct cell** map, struct bot* Saken, int y, int x, int deep, int h, int w);

void anti_trap(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w);
int find_anti_trap(struct cell** map, struct pos* tmp_aim, int y, int x, int fin_value, int h, int w, int* anti_timeout);

void final_enemy_map(struct cell** map, struct bot* Saken, int h, int w, struct pos* enemy_aim);

void adjust_priority(struct cell** map, struct bot* Deidara, struct bot* Saken, struct situation* status, struct pos* enemy_aim, int h, int w);
void adjust_box(struct cell** map, struct bot* Deidara, struct pos* enemy_aim, int y, int x, int h, int w, struct situation* status);
int adjust_one_way(struct cell** map, struct situation* status, struct pos* enemy_aim, int y, int x, int h, int w);
void adjust_quadrant(struct cell** map, struct bot* Deidara, struct situation* status, struct pos* enemy_aim, int y, int x, int h, int w);

void fill_surround(struct cell** map, int h, int w);
int box_arround(struct cell** map, int y, int x, int h, int w);

int find_enemy_dir(struct cell** map, struct bot* Deidara, struct pos* enemy_aim, struct pos* enemy_aim_2, int h, int w, struct pos* trap_pos, struct pos* die_pos);

void escape_teleport(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct pos* escape_pos);

void final_map(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct pos* aim, struct pos* enemy_aim, int* min_tick);

void find_tunnel_kill(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* enemy_aim, struct situation* status, int h, int w, struct pos* tunnel_kill_pos, struct pos* tunnel_die_pos);
int tunnels_around(struct cell** map, struct bot* Saken, int h, int w, struct pos* tmp_pos);
int find_trap_pos(struct cell** map, struct pos* tmp_aim, int y, int x, int fin_value, int h, int w, int* anti_timeout);

int check_potential(struct cell** map, struct bot* Saken, int h, int w, struct pos* art_pos);
int check_around(struct cell** map, int y, int x, int h, int w);

void find_dir(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* aim, int h, int w, int* dir, int current_number, struct situation* status, struct pos* enemy_aim);
void get_tp(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir, int current_number);
int tunnel_kill_acquired(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* tunnel_kill_pos, struct pos* tunnel_die_pos, int h, int w, int* dir, struct situation* status);
int assassin_acquired(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* trap_pos, struct pos* die_pos, int h, int w, int* dir, struct situation* status);
int get_out(struct cell** map, struct bot* Deidara, int* dir, int h, int w, int tick);
int safe_suicide(struct cell** map, struct bot* Deidara, int h, int w);

void print_map(struct cell** map, int h, int w, int map_index);
void print_map_2(struct cell** map, int h, int w, int map_index);
void freentf(struct cell** map, int h, int w, struct pos* aim, struct pos* enemy_aim, int* sector_count, struct pos* tp_aim);
int my_abs_dif(int n1, int n2);
int my_abs(int n);


int main(void)
{
    struct bot* Deidara = malloc(sizeof(struct bot));
    Deidara->f_r = 2;
    Deidara->a_a = 1;
    Deidara->f_t = 0;
    Deidara->f_j = 0;
    Deidara->box_count = 0;
    struct bot* Saken = malloc(sizeof(struct bot));
    Saken->f_r = 2;
    Saken->a_a = 1;
    Saken->x = -1;
    Saken->f_t = 0;
    Saken->f_j = 0;
    Saken->box_count = 0;
    int box_number = 0;
    struct situation* status = malloc(sizeof(struct situation));
    status->half_game = 0;
    status->p_sector = -1;
    status->teleport = 0;
    status->tunnel_wait = 0;
    status->tunnel_kill = 0;
    status->super = 0;
    status->ART = 0;
    status->potential = 0;
    status->explode = 0;
    status->out_of_p_sector = 0;
    status->go_farm = 0;
    struct pos* trap_pos = malloc(sizeof(struct pos));
    struct pos* die_pos = malloc(sizeof(struct pos));
    struct pos* decoy_pos = malloc(sizeof(struct pos));
    struct pos* tp_on_map_pos = malloc(sizeof(struct pos));
    struct pos* tunnel_kill_pos = malloc(sizeof(struct pos));
    struct pos* tunnel_die_pos = malloc(sizeof(struct pos));
    struct pos** late_sequence = malloc(sizeof(struct pos) * 50);
    int late_number = 0;
    while (1)
    {
        int h, w, player_id, tick;
        scanf("%d%d%d%d", &w, &h, &player_id, &tick);

        if (status->tunnel_kill == 1)
        {
            fprintf(stderr, "FOUND A CASE TO TUNNEL KILL\n");
        }
        /*
        Read map
        */
        struct cell** map = malloc(sizeof(struct cell*) * h);
        int current_number = 0;
        int future_number = 0;
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
        Read entities
        */
        int n;
        scanf("%d", &n);
        getchar();
        status->tp_on_map = 0;
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
            fill_entity(map, Deidara, Saken, ent_type, p_id, x, y, param_1, param_2, player_id, h, w, coef, &min_tick, status, tp_on_map_pos);
        }

        /*
        future_sector_count
        */
        future_sector_count(map, &future_number, sector_count, h, w);

        /*
        Quadrant priority
        */
        if (tick == 1)
        {
            current_number = box_number;
        }
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

        out_of_p_sector(map, Deidara, status, h, w);

        if (status->out_of_p_sector == 1 && (current_number * 100 / box_number) < 40)
        {
            status->go_farm = 1;
        }

        /*
        Adjust bomb goal
        */
        adjust_bomb_goal(map, Deidara, h, w);

        /*
        Fill bomb_id
        */

        fill_bomb_Saken(map, player_id, h, w);
        fill_bomb_Deidara(map, player_id, h, w);

        /*
        Count box points
        */
        Deidara->future_box_count = 0;
        Saken->future_box_count = 0;
        count_box_points(map, Deidara, Saken, player_id, h, w);

        /*
        Read features
        */
        Deidara->f_t = 0;
        Deidara->f_j = 0;
        Saken->f_t = 0;
        Saken->f_j = 0;
        status->farm = 0;
        status->super = 0;
        int f_n = 0;
        scanf("%d", &f_n);
        getchar();
        for (int i = 0; i < f_n; i++) {
            int p_id, type;
            scanf("%d%d", &p_id, &type);
            read_features(map, Deidara, Saken, player_id, p_id, type);
        }
        if (Deidara->f_t == 1)
        {
            status->super = 1;
        }
        else if (Saken->f_j == 1 || Saken->f_t == 1)
        {
            status->farm = 1;
            status->tunnel_kill = 0;
        }
        else if (Saken->f_j == 0 && Saken->f_t == 0 && Deidara->f_t == 1 && Deidara->f_j == 1)
        {
            status->super = 1;
        }
        else if (Saken->f_j == 0 && Saken->f_t == 0 && Deidara->f_t == 1 && Deidara->f_j == 0 && Saken->future_box_count + 1 < Deidara->future_box_count)
        {
            status->super = 1;
        }
        else if (Saken->f_j == 0 && Saken->f_t == 0 && Deidara->f_t == 1 &&
            Deidara->future_box_count + 2 < Saken->future_box_count && current_number < 5)
        {
            status->super = 1;
        }

        /*
        Fill path
        */
        fill_path(map, Deidara, Saken, h, w);

        /*
        Feature goal values
        */
        add_feature_goal(map, Deidara, Saken, h, w, status, coef);

        /*
        Fill goal
        */
        fill_goal(map, Deidara, h, w, status);

        /*
        fill_enemy_goal
        */
        if (Saken->x != -1)
        {
            fill_enemy_goal(map, Saken, h, w);
        }


        /*
        Fill positive goal if tick > 4
        */
        fill_extragoal(map, Deidara, player_id, h, w);

        /*
        Fill_features
        */
        fill_features(map, Deidara, h, w);

        /*
        Fill tunnel
            and
        Fill enemy path
            and
        Fill enemy tunnel
        */
        if (Saken->x != -1)
        {
            fill_tunnel(map, Saken, h, w);
            fill_enemy_path(map, Deidara, Saken, h, w);
            fill_enemy_tunnel(map, Saken, h, w);
        }

        /*
        Teleport options
        */
        struct pos* tp_aim = malloc(sizeof(struct pos)); // TEMPORARY SOLUTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        struct pos* escape_pos = malloc(sizeof(struct pos));
        escape_teleport(map, Deidara, Saken, h, w, escape_pos);
        if (status->tp_on_map == 1)
        {
            tp_aim->y = tp_on_map_pos->y;
            tp_aim->x = tp_on_map_pos->x;
        }
        else
        {
            tp_aim->y = escape_pos->y;
            tp_aim->x = escape_pos->x;
        }

        /*
        Final enemy map and adjust goal
        */
        struct pos* enemy_aim = malloc(sizeof(struct pos));
        if (Saken->x != -1)
        {
            enemy_aim->y = Saken->y;
            enemy_aim->x = Saken->x;
            final_enemy_map(map, Saken, h, w, enemy_aim);
            fprintf(stderr, "EA %d %d\n", enemy_aim->y, enemy_aim->x);
            if (current_number > 7)
            {
                adjust_priority(map, Deidara, Saken, status, enemy_aim, h, w);
            }
            // print_map(map, h, w, 8);
        }







        print_map(map, h, w, 14);








        /*
        Change goals if dangerous tunnel
        */
        if (Saken->x != -1)
        {
            anti_trap(map, Deidara, Saken, h, w);
        }

        /*
        Fill Surround
        */
        fill_surround(map, h, w);


        /*
        Fill final goal
        */
        int dir = 5;
        struct pos* aim = malloc(sizeof(struct pos));
        aim->y = Deidara->y;
        aim->x = Deidara->x;
        final_map(map, Deidara, Saken, h, w, aim, enemy_aim, &min_tick);
        //print_map(map, h, w, 8);

        /*
        Tunnel kill case
        */
        if (Saken->x != -1)
        {
            find_tunnel_kill(map, Deidara, Saken, enemy_aim, status, h, w, tunnel_kill_pos, tunnel_die_pos);
        }

        /*
        Suicide kill
        */
        struct pos* art_pos = malloc(sizeof(struct pos));
        art_pos->x = -1;
        art_pos->y = -1;
        if (status->super == 1 && Deidara->f_a > 0)
        {
            fprintf(stderr, "debugger0\n");
            if (map[Saken->y][Saken->x].bomb_tick == 4)
            {
                fprintf(stderr, "debugger1\n");
                status->potential = 1;
            }
            else if (status->potential == 1)
            {
                if (map[Saken->y][Saken->x].surround == 3 && map[Saken->y][Saken->x].box == -3)
                {
                    if (check_potential(map, Saken, h, w, art_pos) == 1)
                    {
                        fprintf(stderr, "debugger2\n");
                        status->ART = 1;
                        status->teleport = 1;
                    }
                }
                status->potential = 0;
            }
            else
            {
                status->potential = 0;
            }
        }

        /*
        Fill the direction
        */
        if (status->explode == 1)
        {
            dir = 4;
            status->explode = 0;
        }
        else if (status->teleport == 1)
        {
            if (status->ART == 1)
            {
                dir = 7;
                tp_aim->y = art_pos->y;
                tp_aim->x = art_pos->x;
                status->ART = 0;
                status->explode = 1;
            }
            else if (status->farm == 1 && status->go_farm == 1 && map[escape_pos->y][escape_pos->x].path > 5)
            {
                dir = 7;
                tp_aim->y = escape_pos->y;
                tp_aim->x = escape_pos->x;
                status->go_farm = 0;
            }
            else if (status->tunnel_kill == 1 && Saken->f_t == 0 && Saken->f_j == 0)
            {
                dir = 7;
                tp_aim->y = tunnel_kill_pos->y;
                tp_aim->x = tunnel_kill_pos->x;
            }
            else if (status->tp_on_map == 1)
            {
                dir = 7;
                tp_aim->y = tp_on_map_pos->y;
                tp_aim->x = tp_on_map_pos->x;
                status->tp_on_map = 0;
                status->tunnel_kill = 0;
            }
            else if (status->tunnel_kill == 1)
            {
                dir = 7;
                tp_aim->y = tunnel_kill_pos->y;
                tp_aim->x = tunnel_kill_pos->x;
            }
            Deidara->f_t = 0;
            status->teleport = 0;
        }
        else if (status->tunnel_kill == 1 && Saken->f_t == 0 && Saken->f_j == 0)
        {
            fprintf(stderr, "Final aim to tunnel kill is %d %d %d Enemy %d %d\n", map[tunnel_kill_pos->y][tunnel_kill_pos->x].goal, tunnel_kill_pos->y, tunnel_kill_pos->x, tunnel_die_pos->y, tunnel_die_pos->x);
            int failure = tunnel_kill_acquired(map, Deidara, Saken, tunnel_kill_pos, tunnel_die_pos, h, w, &dir, status);
            if (failure == 1)
            {
                fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status, enemy_aim);
            }
        }
        else if (status->tp_on_map == 1 && map[tp_on_map_pos->y][tp_on_map_pos->x].path < map[tp_on_map_pos->y][tp_on_map_pos->x].enemy_path - 1 && Deidara->f_t == 0 && current_number >= 10)
        {
            find_dir(map, Deidara, Saken, tp_on_map_pos, h, w, &dir, current_number, status, enemy_aim);
        }
        else if (status->tp_on_map == 1 && map[tp_on_map_pos->y][tp_on_map_pos->x].path < map[tp_on_map_pos->y][tp_on_map_pos->x].enemy_path && Deidara->f_t == 0 && current_number >= 10)
        {
            get_tp(map, Deidara, tp_on_map_pos, h, w, &dir, current_number);
        }
        else if (status->tunnel_kill == 1)
        {
            fprintf(stderr, "Final aim to tunnel kill is %d %d %d Enemy %d %d\n", map[tunnel_kill_pos->y][tunnel_kill_pos->x].goal, tunnel_kill_pos->y, tunnel_kill_pos->x, tunnel_die_pos->y, tunnel_die_pos->x);
            int failure = tunnel_kill_acquired(map, Deidara, Saken, tunnel_kill_pos, tunnel_die_pos, h, w, &dir, status);
            if (failure == 1)
            {
                fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status, enemy_aim);
            }
        }
        else if (map[Deidara->y][Deidara->x].bomb_tick == 5 && Saken->f_t == 1)
        {
            dir = 5;
        }
        else
        {
            fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
            find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status, enemy_aim);
            status->tunnel_kill = 0;
        }


        /*
        Bot actions
        */
        const char actions[][10] = { "left", "right", "up", "down", "bomb", "stay", "jump", "tp" };
        if (dir == 7)
        {
            fprintf(stderr, "tp %d %d\n", tp_aim->x, tp_aim->y);
            printf("tp %d %d\n", tp_aim->x, tp_aim->y);
        }
        else
        {
            fprintf(stderr, "%s", actions[dir]);
            printf("%s\n", actions[dir]);
        }
        fflush(stdout);

        /*
        Free
        */
        free(art_pos);
        free(escape_pos);
        freentf(map, h, w, aim, enemy_aim, sector_count, tp_aim);
    }
    free(tunnel_die_pos);
    free(tunnel_kill_pos);
    free(tp_on_map_pos);
    free(decoy_pos);
    free(trap_pos);
    free(die_pos);
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
        map[i][j].wood_box = 1000;
        break;

    case ';':
        map[i][j].box = 10;
        map[i][j].wood_box = 10;
        (*box_number)++;
        if (i < h / 2 && j < w / 2)
        {
            sector_count[0]++;
        }
        if (i < h / 2 && j > w / 2)
        {
            sector_count[1]++;
        }
        if (i > h / 2 && j > w / 2)
        {
            sector_count[2]++;
        }
        if (i > h / 2 && j < w / 2)
        {
            sector_count[3]++;
        }
        break;

    case '!':
        map[i][j].box = 100;
        map[i][j].wood_box = 100;
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
    map[i][j].bomb_radius = 0;
    map[i][j].bomb_tick = 0;
    map[i][j].surround = 0;
    map[i][j].enemy_goal_2 = 0;
    map[i][j].enemy_path_2 = 1000;
    map[i][j].enemy_tunnel_2 = 0;
    map[i][j].extragoal = 0;
    map[i][j].adjust = 0;
    map[i][j].bomb_Saken = 0;
    map[i][j].bomb_Deidara = 0;
}



void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w, int coef, int* min_tick, struct situation* status, struct pos* tp_on_map_pos)
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
            //fprintf(stderr, "Deidara: x - %d, y - %d, magazine - %d, power = %d\n", Deidara->x, Deidara->y, Deidara->f_a, Deidara->f_r);
        }
        else
        {
            Saken->x = x;
            Saken->y = y;
            Saken->f_a = param_1;
            Saken->f_r = param_2;
            //fprintf(stderr, "Saken: x - %d, y - %d, magazine - %d, power = %d\n", Saken->x, Saken->y, Saken->f_a, Saken->f_r);
        }
        break;

    case 'b':
        fill_bomb(map, Deidara, Saken, p_id, x, y, param_1, param_2, h, w, player_id, min_tick);
        //fprintf(stderr, "bomb: x - %d, y - %d, tick - %d, radius - %d\n", x, y, param_1, param_2);
        break;

    case 'a':
        fill_feature(map, Deidara, Saken, status, ent_type, x, y, coef, h, w);
        //fprintf(stderr, "Feature amount: x - %d, y - %d\n", x, y);
        break;

    case 'l':
        fill_feature(map, Deidara, Saken, status, ent_type, x, y, coef, h, w);
        //fprintf(stderr, "Feature radius: x - %d, y - %d\n", x, y);
        break;

    case 't':
        fill_feature(map, Deidara, Saken, status, ent_type, x, y, coef, h, w);
        if (Deidara->f_t == 1 && Saken->f_t == 0)
        {
            status->teleport = 1;
        }
        status->tp_on_map = 1;
        tp_on_map_pos->x = x;
        tp_on_map_pos->y = y;
        //fprintf(stderr, "Feature teleport: x - %d, y - %d\n", x, y);
        break;

    case 'j':
        fill_feature(map, Deidara, Saken, status, ent_type, x, y, coef, h, w);
        //fprintf(stderr, "Feature jump: x - %d, y - %d\n", x, y);
        break;

    default: break;
    }
}

void fill_bomb(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int* min_tick)
{
    int less_than = 0;
    if (player_id == p_id)
    {
        less_than = 5 - Deidara->f_r;
        map[y][x].wood_box = player_id;
    }
    else
    {
        less_than = 5 - Saken->f_r;
        map[y][x].wood_box = p_id;
    }

    map[y][x].bomb_radius = param_2;

    if (player_id == p_id && Deidara->f_a == 0)
    {
        if (*min_tick == 0 || *min_tick > param_1)
        {
            *min_tick = param_1;
        }
    }

    int tick = param_1;
    if (map[y][x].box < 0 && tick > -1 * map[y][x].box)
    {
        tick = -1 * map[y][x].box;
    }
    map[y][x].bomb_tick = tick;

    if (tick >= less_than && tick != 1)
    {
        map[y][x].box = 50;
    }
    else
    {
        map[y][x].box = -1 * tick;
    }
    map[y][x].goal = -150;

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;
        if (!fill_bomb_dir(map, Deidara, Saken, p_id, tmp_x, y, tick, param_2, h, w, player_id, less_than, min_tick)) break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;
        if (!fill_bomb_dir(map, Deidara, Saken, p_id, tmp_x, y, tick, param_2, h, w, player_id, less_than, min_tick)) break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;
        if (!fill_bomb_dir(map, Deidara, Saken, p_id, x, tmp_y, tick, param_2, h, w, player_id, less_than, min_tick)) break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;
        if (!fill_bomb_dir(map, Deidara, Saken, p_id, x, tmp_y, tick, param_2, h, w, player_id, less_than, min_tick)) break;
    }
}

int fill_bomb_dir(struct cell** map, struct bot* Deidara, struct bot* Saken, int p_id, int x, int y, int param_1, int param_2, int h, int w, int player_id, int less_than, int* min_tick)
{
    if (map[y][x].box == 1)
    {
        if (param_1 >= 2)
            map[y][x].goal = -150;
        map[y][x].box = -1 * param_1;
    }
    else if (map[y][x].bomb_radius != 0)
    {
        if (map[y][x].bomb_tick > param_1)
        {
            fill_bomb(map, Deidara, Saken, p_id, x, y, param_1, map[y][x].bomb_radius, h, w, player_id, min_tick);
            return 0;
        }
    }
    else if (map[y][x].box < 0)
    {
        if (map[y][x].box < -1 * param_1 && map[y][x].bomb_radius == 0)
        {
            if (param_1 >= 2)
                map[y][x].goal = -150;
            map[y][x].box = -1 * param_1;
        }
    }
    else if (map[y][x].box == 10)
    {
        if (param_1 >= less_than)
        {
            map[y][x].box = 50;
        }
        else
        {
            map[y][x].box = -1 * param_1;
        }
        return 0;
    }
    else
    {
        return 0;
    }
    return 1;
}

void fill_feature(struct cell** map, struct bot* Deidara, struct bot* Saken, struct situation* status, char ent_type, int x, int y, int coef, int h, int w)
{
    if (Deidara->f_a > Deidara->a_a)
        Deidara->a_a = Deidara->f_a;
    //fprintf(stderr, "COEFF and a_a %d %d \n", 8 * coef / 100, Deidara->a_a);
    if (ent_type == 'a')
    {
        map[y][x].enemy_goal = 4;
        map[y][x].features = -1;
    }
    else if (ent_type == 'l')
    {
        map[y][x].enemy_goal = 4;
        map[y][x].features = -2;
    }
    else if (ent_type == 't')
    {
        map[y][x].features = -10;
        if (Saken->f_t == 0)
        {
            map[y][x].enemy_goal = 8;
        }
    }
    else if (ent_type == 'j')
    {
        map[y][x].features = -9;
        if (Saken->f_j == 0)
        {
            map[y][x].enemy_goal = 6;
        }
    }
}

void future_sector_count(struct cell** map, int* future_number, int* sector_count, int h, int w)
{
    int index = 0;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].box == 10)
            {
                if (i < h / 2 && j < w / 2)
                {
                    sector_count[0]++;
                }
                else if (i < h / 2 && j > w / 2)
                {
                    sector_count[1]++;
                }
                else if (i > h / 2 && j > w / 2)
                {
                    sector_count[2]++;
                }
                else
                {
                    sector_count[3]++;
                }
                index++;
            }
        }
    }
    if (index != 0) index--;
    *future_number = index;
}

void out_of_p_sector(struct cell** map, struct bot* Deidara, struct situation* status, int h, int w)
{
    status->out_of_p_sector = 0;
    switch (status->p_sector)
    {
    case 0:
        if (Deidara->x > h / 2 || Deidara->y > w / 2) status->out_of_p_sector = 1;
        break;
    case 1:
        if (Deidara->x > h / 2 || Deidara->y < w / 2) status->out_of_p_sector = 1;
        break;
    case 2:
        if (Deidara->x < h / 2 || Deidara->y < w / 2) status->out_of_p_sector = 1;
        break;
    case 3:
        if (Deidara->x < h / 2 || Deidara->y > w / 2) status->out_of_p_sector = 1;
        break;
    default:
        status->out_of_p_sector = 0;
        break;
    }
}



void adjust_bomb_goal(struct cell** map, struct bot* Deidara, int h, int w)
{
    if (map[Deidara->y][Deidara->x].box != 50 && map[Deidara->y][Deidara->x].box >= 1)
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                if (map[i][j].goal == -150)
                {
                    map[i][j].goal = 0;
                }
            }
        }
    }
}



void fill_bomb_Saken(struct cell** map, int player_id, int h, int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].bomb_tick != 0)
            {
                if (map[i][j].wood_box != player_id)
                {
                    get_bomb_Saken(map, j, i, h, w);
                }
            }
        }
    }
}

void get_bomb_Saken(struct cell** map, int x, int y, int h, int w)
{
    map[y][x].bomb_Saken = -1;
    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (x + i < w)
        {
            if (map[y][x + i].wood_box == 1000) map[y][x + i].bomb_Saken = -1;
            else if (map[y][x + i].wood_box == 100) break;
            else if (map[y][x + i].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (x - i >= 0)
        {
            if (map[y][x - i].wood_box == 1000) map[y][x - i].bomb_Saken = -1;
            else if (map[y][x - i].wood_box == 100) break;
            else if (map[y][x - i].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (y + i < h)
        {
            if (map[y + i][x].wood_box == 1000) map[y + i][x].bomb_Saken = -1;
            else if (map[y + i][x].wood_box == 100) break;
            else if (map[y + i][x].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (y - i >= 0)
        {
            if (map[y - i][x].wood_box == 1000) map[y - i][x].bomb_Saken = -1;
            else if (map[y - i][x].wood_box == 100) break;
            else if (map[y - i][x].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }
}



void fill_bomb_Deidara(struct cell** map, int player_id, int h, int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].bomb_tick != 0)
            {
                if (map[i][j].wood_box == player_id)
                {
                    get_bomb_Deidara(map, j, i, h, w);
                }
            }
        }
    }
}

void get_bomb_Deidara(struct cell** map, int x, int y, int h, int w)
{
    map[y][x].bomb_Deidara = -1;
    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (x + i < w)
        {
            if (map[y][x + i].wood_box == 1000) map[y][x + i].bomb_Deidara = -1;
            else if (map[y][x + i].wood_box == 100) break;
            else if (map[y][x + i].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (x - i >= 0)
        {
            if (map[y][x - i].wood_box == 1000) map[y][x - i].bomb_Deidara = -1;
            else if (map[y][x - i].wood_box == 100) break;
            else if (map[y][x - i].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (y + i < h)
        {
            if (map[y + i][x].wood_box == 1000) map[y + i][x].bomb_Deidara = -1;
            else if (map[y + i][x].wood_box == 100) break;
            else if (map[y + i][x].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i <= map[y][x].bomb_radius; i++)
    {
        if (y - i >= 0)
        {
            if (map[y - i][x].wood_box == 1000) map[y - i][x].bomb_Deidara = -1;
            else if (map[y - i][x].wood_box == 100) break;
            else if (map[y - i][x].wood_box == 10) break;
        }
        else
        {
            break;
        }
    }
}



void count_box_points(struct cell** map, struct bot* Deidara, struct bot* Saken, int player_id, int h, int w)
{
    int additional_box_count_Deidara = 0;
    int additional_box_count_Saken = 0;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].bomb_tick == 1)
            {
                if (map[i][j].wood_box == player_id)
                {
                    get_box_points(map, Deidara, j, i, h, w);
                }
                else
                {
                    get_box_points(map, Saken, j, i, h, w);
                }
            }
            else if (map[i][j].bomb_tick > 1 && map[i][j].bomb_tick < 6)
            {
                if (map[i][j].wood_box == player_id)
                {
                    get_future_box_points(map, Deidara, j, i, h, w, &additional_box_count_Deidara);
                }
                else
                {
                    get_future_box_points(map, Saken, j, i, h, w, &additional_box_count_Saken);
                }
            }
        }
    }
    Deidara->future_box_count = Deidara->box_count + additional_box_count_Deidara;
    Saken->future_box_count = Saken->box_count + additional_box_count_Saken;
}

void get_box_points(struct cell** map, struct bot* bot, int x, int y, int h, int w)
{
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (x + i < w)
        {
            if (map[y][x + i].wood_box == 10)
            {
                bot->box_count++;
                break;
            }
            if (map[y][x + i].wood_box == 100)
            {
                break;
            }
        }
    }
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (y + i < h)
        {
            if (map[y + i][x].wood_box == 10)
            {
                bot->box_count++;
                break;
            }
            if (map[y + i][x].wood_box == 100)
            {
                break;
            }
        }
    }
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (x - i >= 0)
        {
            if (map[y][x - i].wood_box == 10)
            {
                bot->box_count++;
                break;
            }
            if (map[y][x - i].wood_box == 100)
            {
                break;
            }
        }
    }
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (y - i >= 0)
        {
            if (map[y - i][x].wood_box == 10)
            {
                bot->box_count++;
                break;
            }
            if (map[y - i][x].wood_box == 100)
            {
                break;
            }
        }
    }
}

void get_future_box_points(struct cell** map, struct bot* bot, int x, int y, int h, int w, int* additional_box_count)
{
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (x + i < w)
        {
            if (map[y][x + i].wood_box == 10)
            {
                *additional_box_count += 1;
                break;
            }
            else if (map[y][x + i].wood_box == 100)
            {
                break;
            }
        }
    }
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (y + i < h)
        {
            if (map[y + i][x].wood_box == 10)
            {
                *additional_box_count += 1;
                break;
            }
            else if (map[y + i][x].wood_box == 100)
            {
                break;
            }
        }
    }
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (x - i >= 0)
        {
            if (map[y][x - i].wood_box == 10)
            {
                *additional_box_count += 1;
                break;
            }
            else if (map[y][x - i].wood_box == 100)
            {
                break;
            }
        }
    }
    for (int i = 1; i <= bot->f_r; i++)
    {
        if (y - i >= 0)
        {
            if (map[y - i][x].wood_box == 10)
            {
                *additional_box_count += 1;
                break;
            }
            else if (map[y - i][x].wood_box == 100)
            {
                break;
            }
        }
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
            {
                fill_box(map, Deidara, i, j, h, w, status, value);
                if (status->half_game == 0)
                {
                    if (i >= 3 && i <= 7 && j >= 4 && j <= 8)
                    {
                        fill_box(map, Deidara, i, j, h, w, status, quadrant_value / 2);
                    }
                }
                else if (status->half_game == 1)
                {
                    switch (status->p_sector)
                    {
                    case 0:
                        if (i < (h / 2) + 1 && j < (w / 2) + 1)
                        {
                            fill_box(map, Deidara, i, j, h, w, status, quadrant_value);
                        }
                        break;
                    case 1:
                        if (i < (h / 2) + 1 && j >(w / 2) - 1)
                        {
                            fill_box(map, Deidara, i, j, h, w, status, quadrant_value);
                        }
                        break;
                    case 2:
                        if (i > (h / 2) - 1 && j > (w / 2) - 1)
                        {
                            fill_box(map, Deidara, i, j, h, w, status, quadrant_value);
                        }
                        break;
                    case 3:
                        if (i > (h / 2) - 1 && j < (w / 2) + 1)
                        {
                            fill_box(map, Deidara, i, j, h, w, status, quadrant_value);
                        }
                        break;
                    default:
                        if (i >= 3 && i <= 7 && j >= 4 && j <= 8)
                        {
                            fill_box(map, Deidara, i, j, h, w, status, quadrant_value);
                        }
                        break;
                    }
                }
            }
        }
    }
}

void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w, struct situation* status, int goal_val)
{
    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal += goal_val;

            // quadrant_priority(map, status, y, tmp_x, h, w);
        }
        else if (map[y][tmp_x].box < 1 || map[y][tmp_x].box == 50)
        {
            continue;
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
            map[y][tmp_x].goal += goal_val;

            // quadrant_priority(map, status, y, tmp_x, h, w);
        }
        else if (map[y][tmp_x].box < 1 || map[y][tmp_x].box == 50)
        {
            continue;
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
            map[tmp_y][x].goal += goal_val;

            // quadrant_priority(map, status, tmp_y, x, h, w);
        }
        else if (map[tmp_y][x].box < 1 || map[tmp_y][x].box == 50)
        {
            continue;
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
            map[tmp_y][x].goal += goal_val;

            // quadrant_priority(map, status, tmp_y, x, h, w);
        }
        else if (map[tmp_y][x].box < 1 || map[tmp_y][x].box == 50)
        {
            continue;
        }
        else
            break;
    }
}

// void quadrant_priority(struct cell** map, struct situation* status, int i, int j, int h, int w)
// {
//     int added_priority = 4;
    // if (status->half_game == 0 && map[i][j].goal > 0)
    // {
    //     if (i >= 3 && i <= 7 && j >= 4 && j <= 8)
    //     {
    //         map[i][j].goal += added_priority / 2;
    //     }
    // }
    // else if (status->half_game == 1 && map[i][j].goal > 0)
    // {
    //     switch (status->p_sector)
    //     {
    //     case 0:
    //         if (i < (h / 2) + 1 && j < (w / 2) + 1)
    //         {
    //             map[i][j].goal += added_priority;
    //         }
    //         break;
    //     case 1:
    //         if (i < (h / 2) + 1 && j > (w / 2) - 1)
    //         {
    //             map[i][j].goal += added_priority;
    //         }
    //         break;
    //     case 2:
    //         if (i > (h / 2) - 1 && j > (w / 2) - 1)
    //         {
    //             map[i][j].goal += added_priority;
    //         }
    //         break;
    //     case 3:
    //         if (i > (h / 2) - 1 && j < (w / 2) + 1)
    //         {
    //             map[i][j].goal += added_priority;
    //         }
    //         break;
    //     default:
    //         if (i >= 3 && i <= 7 && j >= 4 && j <= 8)
    //         {
    //             map[i][j].goal += added_priority;
    //         }
    //         break;
    //     }
    // }
// }


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
        else if (map[y][tmp_x].box < 0 && map[y][tmp_x].box == 50)
        {
            continue;
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
        else if (map[y][tmp_x].box < 0 && map[y][tmp_x].box == 50)
        {
            continue;
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
        else if (map[tmp_y][x].box < 0 && map[tmp_y][x].box == 50)
        {
            continue;
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
        else if (map[tmp_y][x].box < 0 && map[tmp_y][x].box == 50)
        {
            continue;
        }
        else
            break;
    }
}



void fill_path(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Deidara->x;
    int y = Deidara->y;
    int bomb = 0;
    get_point(map, Deidara, Saken, y, x, step, prev, h, w, bomb);
}

void get_point(struct cell** map, struct bot* Deidara, struct bot* Saken, int y, int x, int step, int prev, int h, int w, int bomb)
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

    if (step == 0)
    {
        map[y][x].path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box == 1 && step < map[y][x].path)
    {
        //not allow to get trapped
        if (map[Deidara->y][Deidara->x].bomb_tick != 0 && step == 2)
        {
            if (!(Saken->y == Deidara->y && Saken->x == Deidara->x))
            {
                if (Saken_nearby(map, Saken, y, x, h, w) == 1)
                {
                    return;
                }
            }
        }
        map[y][x].path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box < 0 && step < map[y][x].path)
    {
        //not allow to get trapped
        if (map[Deidara->y][Deidara->x].bomb_tick != 0 && step == 2)
        {
            if (!(Saken->y == Deidara->y && Saken->x == Deidara->x))
            {
                if (Saken_nearby(map, Saken, y, x, h, w) == 1)
                {
                    return;
                }
            }
        }
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
        get_point(map, Deidara, Saken, y, x + 1, step, 'R', h, w, bomb);
    if (prev != 'U' && y + 1 < h)
        get_point(map, Deidara, Saken, y + 1, x, step, 'D', h, w, bomb);
    if (prev != 'R' && x - 1 >= 0)
        get_point(map, Deidara, Saken, y, x - 1, step, 'L', h, w, bomb);
    if (prev != 'D' && y - 1 >= 0)
        get_point(map, Deidara, Saken, y - 1, x, step, 'U', h, w, bomb);
}

int Saken_nearby(struct cell** map, struct bot* Saken, int y, int x, int h, int w)
{
    if (Saken->y == y && Saken->x == x)
    {
        return 1;
    }
    if (y - 1 >= 0)
    {
        if (Saken->y == y - 1 && Saken->x == x)
            return 1;
    }
    if (y + 1 < h)
    {
        if (Saken->y == y + 1 && Saken->x == x)
            return 1;
    }
    if (x - 1 >= 0)
    {
        if (Saken->y == y && Saken->x == x - 1)
            return 1;
    }
    if (x + 1 < w)
    {
        if (Saken->y == y && Saken->x == x + 1)
            return 1;
    }
    return 0;
}

void add_feature_goal(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct situation* status, int coef)
{
    int amount_value = 15;
    int radius_value = 15;
    int teleport_value = 35;
    int jump_value = 15;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].features < 0)
            {
                switch (map[i][j].features)
                {
                case -1:
                    if (Deidara->a_a == 1)
                    {
                        add_feat_goal(map, Deidara, Saken, i, j, h, w, status, amount_value, coef);
                    }
                    break;
                case -2:
                    if (Deidara->f_r <= 3)
                    {
                        add_feat_goal(map, Deidara, Saken, i, j, h, w, status, radius_value, coef);
                    }
                    break;
                case -9:
                    if ((Deidara->f_t == 1 && Saken->f_j == 0 && Saken->f_t == 0) || Deidara->f_j == 0)
                    {
                        add_feat_goal(map, Deidara, Saken, i, j, h, w, status, jump_value, coef);
                    }
                    break;
                case -10:
                    map[i][j].goal += 35 * coef / 100;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void add_feat_goal(struct cell** map, struct bot* Deidara, struct bot* Saken, int i, int j, int h, int w, struct situation* status, int feat_value, int coef)
{
    int path_coef = 1;
    if (map[i][j].path < 10)
        path_coef = (10 - map[i][j].path) * 3 / 2;
    fprintf(stderr, "coef - %d\n", coef);
    switch (status->p_sector)
    {
    case 0:
        if (i < (h / 2) + 1 && j < (w / 2) + 1)
        {
            map[i][j].goal += feat_value * coef * path_coef / 1000;
        }
        break;
    case 1:
        if (i < (h / 2) + 1 && j >(w / 2) - 1)
        {
            map[i][j].goal += feat_value * coef * path_coef / 1000;
        }
        break;
    case 2:
        if (i > (h / 2) - 1 && j > (w / 2) - 1)
        {
            map[i][j].goal += feat_value * coef * path_coef / 1000;
        }
        break;
    case 3:
        if (i > (h / 2) - 1 && j < (w / 2) + 1)
        {
            map[i][j].goal += feat_value * coef * path_coef / 1000;
        }
        break;
    default:
        map[i][j].goal += feat_value * coef * path_coef / 1000;
        break;
    }
}

void fill_extragoal(struct cell** map, struct bot* Deidara, int player_id, int h, int w)
{
    if (map[Deidara->y][Deidara->x].box == -5 && map[Deidara->y][Deidara->x].bomb_tick != -5)
    {
        get_extragoal(map, Deidara, player_id, Deidara->x, Deidara->y, h, w);
    }
}

void get_extragoal(struct cell** map, struct bot* Deidara, int player_id, int x, int y, int h, int w)
{
    for (int i = 1; i <= Deidara->f_r; i++)
    {
        if (x + i < w)
        {
            if (map[y][x + i].wood_box == 10)
            {
                get_extragoal_part_2(map, Deidara, player_id, x + i, y, h, w);
                break;
            }
            else if (map[y][x + i].wood_box == 100)
            {
                break;
            }
            else if (map[y][x + i].wood_box == player_id)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        if (x - i >= 0)
        {
            if (map[y][x - i].wood_box == 10)
            {
                get_extragoal_part_2(map, Deidara, player_id, x - i, y, h, w);
                break;
            }
            else if (map[y][x - i].wood_box == 100)
            {
                break;
            }
            else if (map[y][x - i].wood_box == player_id)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        if (y + i < h)
        {
            if (map[y + i][x].wood_box == 10)
            {
                get_extragoal_part_2(map, Deidara, player_id, x, y + i, h, w);
                break;
            }
            else if (map[y + i][x].wood_box == 100)
            {
                break;
            }
            else if (map[y + i][x].wood_box == player_id)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        if (y - i >= 0)
        {
            if (map[y - i][x].wood_box == 10)
            {
                get_extragoal_part_2(map, Deidara, player_id, x, y - i, h, w);
                break;
            }
            else if (map[y - i][x].wood_box == 100)
            {
                break;
            }
            else if (map[y - i][x].wood_box == player_id)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

void get_extragoal_part_2(struct cell** map, struct bot* Deidara, int player_id, int x, int y, int h, int w)
{
    int index = 0;
    for (int i = 1; i <= Deidara->f_r && index == 0; i++)
    {
        if (x + i < w)
        {
            if (map[y][x + i].wood_box == player_id)
            {
                index = 1;
                break;
            }
            else if (map[y][x + i].wood_box == 10)
            {
                break;
            }
            else if (map[y][x + i].wood_box == 100)
            {
                break;
            }
            else if (map[y][x + i].wood_box > 0 && map[y][x + i].wood_box < 10 && map[y][x + i].wood_box != player_id && Deidara->y == y && Deidara->x < x && map[y][x + i].bomb_tick < 5)
            {
                index = 1;
                break;
            }
            else break;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Deidara->f_r && index == 0; i++)
    {
        if (x - i >= 0)
        {
            if (map[y][x - i].wood_box == player_id)
            {
                index = 1;
                break;
            }
            else if (map[y][x - i].wood_box == 10)
            {
                break;
            }
            else if (map[y][x - i].wood_box == 100)
            {
                break;
            }
            else if (map[y][x - i].wood_box > 0 && map[y][x - i].wood_box < 10 && map[y][x - i].wood_box != player_id && Deidara->y == y && Deidara->x > x && map[y][x - i].bomb_tick < 5)
            {
                index = 1;
                break;
            }
            else break;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Deidara->f_r && index == 0; i++)
    {
        if (y + i < h)
        {
            if (map[y + i][x].wood_box == player_id)
            {
                index = 1;
                break;
            }
            else if (map[y + i][x].wood_box == 10)
            {
                break;
            }
            else if (map[y + i][x].wood_box == 100)
            {
                break;
            }
            else if (map[y + i][x].wood_box > 0 && map[y + i][x].wood_box < 10 && map[y + i][x].wood_box != player_id && Deidara->y < y && Deidara->x == x && map[y + i][x].bomb_tick < 5)
            {
                index = 1;
                break;
            }
            else break;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= Deidara->f_r && index == 0; i++)
    {
        if (y - i >= 0)
        {
            if (map[y - i][x].wood_box == player_id)
            {
                index = 1;
                break;
            }
            else if (map[y - i][x].wood_box == 10)
            {
                break;
            }
            else if (map[y - i][x].wood_box == 100)
            {
                break;
            }
            else if (map[y - i][x].wood_box > 0 && map[y - i][x].wood_box < 10 && map[y - i][x].wood_box != player_id && Deidara->y > y && Deidara->x == x && map[y - i][x].bomb_tick < 5)
            {
                index = 1;
                break;
            }
            else break;
        }
        else
        {
            break;
        }
    }

    if (index == 0)
    {
        map[Deidara->y][Deidara->x].extragoal += 4;
    }
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



void fill_enemy_path(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Saken->x;
    int y = Saken->y;
    int bomb = 0;
    get_enemy_point(map, Deidara, Saken, y, x, step, prev, h, w, bomb);
}

void get_enemy_point(struct cell** map, struct bot* Deidara, struct bot* Saken, int y, int x, int step, int prev, int h, int w, int bomb)
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
    else if (map[Saken->y][Saken->x].bomb_radius != 0 && y == Deidara->y && Deidara->x == x)
    {
        return;
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
        get_enemy_point(map, Deidara, Saken, y, x + 1, step, 'R', h, w, bomb);
    if (prev != 'U' && y + 1 < h)
        get_enemy_point(map, Deidara, Saken, y + 1, x, step, 'D', h, w, bomb);
    if (prev != 'R' && x - 1 >= 0)
        get_enemy_point(map, Deidara, Saken, y, x - 1, step, 'L', h, w, bomb);
    if (prev != 'D' && y - 1 >= 0)
        get_enemy_point(map, Deidara, Saken, y - 1, x, step, 'U', h, w, bomb);
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
            adjust_quadrant(map, Deidara, status, enemy_aim, y, tmp_x, h, w);
            break;
        }
        else if (map[y][tmp_x].box == 100)
        {
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
            adjust_quadrant(map, Deidara, status, enemy_aim, y, tmp_x, h, w);
            break;
        }
        else if (map[y][tmp_x].box == 100)
        {
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
            adjust_quadrant(map, Deidara, status, enemy_aim, tmp_y, x, h, w);
            break;
        }
        else if (map[tmp_y][x].box == 100)
        {
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
            adjust_quadrant(map, Deidara, status, enemy_aim, tmp_y, x, h, w);
            break;
        }
        else if (map[tmp_y][x].box == 100)
        {
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
    int good_value = 2;
    int equal_value = 1;
    if (map[y][x].box == 1)
    {
        if (map[y][x].path < map[enemy_aim->y][enemy_aim->x].enemy_path && map[y][x].adjust < good_value)
        {
            map[y][x].adjust += good_value;
        }
        else if (map[y][x].path == map[enemy_aim->y][enemy_aim->x].enemy_path && map[y][x].adjust < equal_value)
        {
            map[y][x].adjust += equal_value;
        }
        else if (map[y][x].path > map[enemy_aim->y][enemy_aim->x].enemy_path)
        {
            map[y][x].goal -= value;
        }
    }
    else if (map[y][x].box < 1 || map[y][x].box == 50)
        return 0;
    else
        return 1;
    return 0;
}

void adjust_quadrant(struct cell** map, struct bot* Deidara, struct situation* status, struct pos* enemy_aim, int y, int x, int h, int w)
{
    int added_value = 0;
    if (status->half_game == 0)
    {
        if (y >= 3 && y <= 7 && x >= 4 && x <= 8)
        {
            added_value = quadrant_value / 2;
        }
    }
    else if (status->half_game == 1)
    {
        switch (status->p_sector)
        {
        case 0:
            if (y < (h / 2) + 1 && x < (w / 2) + 1)
            {
                added_value = quadrant_value;
            }
            break;
        case 1:
            if (y < (h / 2) + 1 && x >(w / 2) - 1)
            {
                added_value = quadrant_value;
            }
            break;
        case 2:
            if (y > (h / 2) - 1 && x > (w / 2) - 1)
            {
                added_value = quadrant_value;
            }
            break;
        case 3:
            if (y > (h / 2) - 1 && x < (w / 2) + 1)
            {
                added_value = quadrant_value;
            }
            break;
        default:
            if (y >= 3 && y <= 7 && x >= 4 && x <= 8)
            {
                added_value = quadrant_value;
            }
            break;
        }
    }

    if (added_value != 0)
    {
        for (int i = 1; i <= Deidara->f_r; i++)
        {
            int tmp_x = x + i;
            if (tmp_x >= w) break;

            if (map[y][tmp_x].box == 1 && map[y][tmp_x].path > map[enemy_aim->y][enemy_aim->x].enemy_path)
            {
                map[y][tmp_x].goal -= added_value;
            }
            else if (map[y][tmp_x].box < 1 || map[y][tmp_x].box == 50)
            {
                continue;
            }
            else
                break;
        }

        for (int i = 1; i <= Deidara->f_r; i++)
        {
            int tmp_x = x - i;
            if (tmp_x < 0) break;

            if (map[y][tmp_x].box <= 1 && map[y][tmp_x].path > map[enemy_aim->y][enemy_aim->x].enemy_path)
            {
                map[y][tmp_x].goal -= added_value;
            }
            else if (map[y][tmp_x].box < 1 || map[y][tmp_x].box == 50)
            {
                continue;
            }
            else
                break;
        }

        for (int i = 1; i <= Deidara->f_r; i++)
        {
            int tmp_y = y + i;
            if (tmp_y >= h) break;

            if (map[tmp_y][x].box <= 1 && map[tmp_y][x].path > map[enemy_aim->y][enemy_aim->x].enemy_path)
            {
                map[tmp_y][x].goal -= added_value;
            }
            else if (map[tmp_y][x].box < 1 || map[tmp_y][x].box == 50)
            {
                continue;
            }
            else
                break;
        }

        for (int i = 1; i <= Deidara->f_r; i++)
        {
            int tmp_y = y - i;
            if (tmp_y < 0) break;

            if (map[tmp_y][x].box <= 1 && map[tmp_y][x].path > map[enemy_aim->y][enemy_aim->x].enemy_path)
            {
                map[tmp_y][x].goal -= added_value;
            }
            else if (map[tmp_y][x].box < 1 || map[tmp_y][x].box == 50)
            {
                continue;
            }
            else
                break;
        }
    }
}



void anti_trap(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w)
{
    int trap_pos = 0;
    struct pos* tmp_aim = malloc(sizeof(struct pos));

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].tunnel < 0)
            {
                if (Deidara->f_j == 0 && Saken->f_t == 1)
                {
                    if (map[i][j].goal > 0)
                    {
                        map[i][j].goal = 0;
                    }
                    continue;
                }
                int anti_timeout = 0;
                int fin_value = my_abs(map[i][j].tunnel) + 1;
                tmp_aim->y = -1;
                tmp_aim->x = -1;
                if (Saken->f_r > Deidara->f_r)
                {
                    fin_value += Saken->f_r;
                }
                else
                {
                    fin_value += Deidara->f_r;
                }

                if (find_anti_trap(map, tmp_aim, i, j, fin_value, h, w, &anti_timeout) == 0)
                {
                    fprintf(stderr, "error in find_trap_pos\n");
                    continue;
                }

                if (tmp_aim->y == -1)
                {
                    continue;
                }

                int safe_path = map[i][j].path + 1 + (map[tmp_aim->y][tmp_aim->x].tunnel % 100) - my_abs(map[i][j].tunnel);

                if (map[tmp_aim->y][tmp_aim->x].enemy_path < safe_path)
                {
                    if (map[i][j].goal > 0)
                    {
                        map[i][j].goal = 0;
                    }
                }
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
            find_anti_trap(map, tmp_aim, y - 1, x, fin_value, h, w, anti_timeout);
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
            find_anti_trap(map, tmp_aim, y + 1, x, fin_value, h, w, anti_timeout);
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
            find_anti_trap(map, tmp_aim, y, x - 1, fin_value, h, w, anti_timeout);
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
            find_anti_trap(map, tmp_aim, y, x + 1, fin_value, h, w, anti_timeout);
        }
    }
    return 1;
}



void fill_surround(struct cell** map, int h, int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].box <= 1)
            {
                map[i][j].surround = box_arround(map, i, j, h, w);
            }
        }
    }
}

int box_arround(struct cell** map, int y, int x, int h, int w)
{
    int boxes = 0;
    if (y - 1 < 0) boxes++;
    else if (map[y - 1][x].box > 1) boxes++;
    if (y + 1 >= h) boxes++;
    else if (map[y + 1][x].box > 1) boxes++;
    if (x - 1 < 0) boxes++;
    else if (map[y][x - 1].box > 1) boxes++;
    if (x + 1 >= w) boxes++;
    else if (map[y][x + 1].box > 1) boxes++;
    return boxes;
}



int find_enemy_dir(struct cell** map, struct bot* Deidara, struct pos* enemy_aim, struct pos* enemy_aim_2, int h, int w, struct pos* trap_pos, struct pos* die_pos)
{
    int anti_timeout = 0;
    int y = enemy_aim_2->y;
    int x = enemy_aim_2->x;
    int wait = 0;
    struct pos next = { -1, -1 };
    struct pos next_next = { -1, -1 };
    int fork = 0;
    int dir = 0;
    while (y != enemy_aim->y || x != enemy_aim->x)
    {
        fork = 0;
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].enemy_path_2 == map[y][x].enemy_path_2 - 1)
            {
                next_next.y = next.y;
                next_next.x = next.x;
                next.y = y;
                next.x = x;
                fork++;
                dir = 1;
            }
        }
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].enemy_path_2 == map[y][x].enemy_path_2 - 1)
            {
                next_next.y = next.y;
                next_next.x = next.x;
                next.y = y;
                next.x = x;
                fork++;
                dir = 3;
            }
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].enemy_path_2 == map[y][x].enemy_path_2 - 1)
            {
                next_next.y = next.y;
                next_next.x = next.x;
                next.y = y;
                next.x = x;
                fork++;
                dir = 0;
            }
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].enemy_path_2 == map[y][x].enemy_path_2 - 1)
            {
                next_next.y = next.y;
                next_next.x = next.x;
                next.y = y;
                next.x = x;
                fork++;
                dir = 2;
            }
        }
        if (dir == 1) x--;
        if (dir == 3) y--;
        if (dir == 0) x++;
        if (dir == 2) y++;
        trap_pos->y = next_next.y;
        trap_pos->x = next_next.x;
        die_pos->y = next.y;
        die_pos->x = next.x;

        if (fork >= 2) return 1;
        if (anti_timeout < 50) anti_timeout++;
        else if (anti_timeout >= 50)
        {
            fprintf(stderr, "ALERT ALERT ALERT ALERT Find_enemy_dir timeout.\n");
            /*if (x - 1 >= 0) //ACTIVATE FOR ARENA
                if (map[y][x - 1].enemy_box_2 <= 1)
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
            return 1;
        }
    }
    return 0;
}

void escape_teleport(struct cell** map, struct bot* Deidara, struct bot* Saken, int h, int w, struct pos* escape_pos)
{
    escape_pos->y = 0;
    escape_pos->x = 0;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[escape_pos->y][escape_pos->x].goal <= map[i][j].goal)
            {
                if ((map[i][j].path != 1000 || map[i][j].enemy_path != 1000) && map[i][j].box == 1)
                {
                    escape_pos->y = i;
                    escape_pos->x = j;
                }
            }
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
                    map[i][j].goal -= (map[i][j].path - *min_tick - map[i][j].adjust);
            }
            else
                map[i][j].goal += map[i][j].adjust - map[i][j].path; //+ up to tick or path
            if (map[i][j].goal > map[aim->y][aim->x].goal)
            {
                aim->y = i;
                aim->x = j;
                //fprintf(stderr, "aim changed to %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
            }
            else if (map[i][j].goal == map[aim->y][aim->x].goal)
            {
                // int middle_x = w / 2;
                // int middle_y = h / 2;
                // int aim_dif = my_abs_dif(aim->y, middle_y) + my_abs_dif(aim->x, middle_x);
                // int cur_dif = my_abs_dif(i, middle_y) + my_abs_dif(j, middle_x);
                // if (cur_dif < aim_dif)
                // {
                //     aim->y = i;
                //     aim->x = j;
                //     //fprintf(stderr, "aim changed to %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                // }
                if (map[i][j].path < map[aim->y][aim->x].path)
                {
                    aim->y = i;
                    aim->x = j;
                }
            }
        }
    }
}



void find_tunnel_kill(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* enemy_aim, struct situation* status, int h, int w, struct pos* tunnel_kill_pos, struct pos* tunnel_die_pos)
{
    struct pos* tmp_aim = malloc(sizeof(struct pos));
    int anti_timeout = 1;
    tmp_aim->y = -1;
    tmp_aim->x = -1;
    if (my_abs(map[Saken->y][Saken->x].tunnel) <= 1)
    {
        anti_timeout = 1;
        tmp_aim->y = -1;
        tmp_aim->x = -1;
        if (find_trap_pos(map, tmp_aim, Saken->y, Saken->x, my_abs(map[Saken->y][Saken->x].tunnel) + 2, h, w, &anti_timeout) == 0)
        {
            fprintf(stderr, "error in find_trap_pos\n");
            free(tmp_aim);
            return;
        }

        if (tmp_aim->y != -1 && tmp_aim->x != -1)
        {
            if (my_abs_dif(tmp_aim->y, Saken->y) + my_abs_dif(tmp_aim->x, Saken->x) == 1)
            {
                if (tmp_aim->y == Deidara->y && tmp_aim->x == Deidara->x &&
                    (map[Deidara->y][Deidara->x].box <= -4 || map[Deidara->y][Deidara->x].box >= 1) &&
                    (map[Deidara->y][Deidara->x].bomb_tick == 0 || map[Deidara->y][Deidara->x].bomb_tick >= 4))
                {
                    fprintf(stderr, "Case tunnel 1\n");
                    status->tunnel_kill = 1;
                    status->tunnel_wait = 0;
                    tunnel_die_pos->y = Saken->y;
                    tunnel_die_pos->x = Saken->x;
                    tunnel_kill_pos->y = tmp_aim->y;
                    tunnel_kill_pos->x = tmp_aim->x;
                }
            }
            else if (my_abs_dif(tmp_aim->y, Saken->y) + my_abs_dif(tmp_aim->x, Saken->x) == 2)
            {
                struct pos* tmp_aim_2 = malloc(sizeof(struct pos));
                int anti_timeout_2 = 1;
                tmp_aim_2->y = -1;
                tmp_aim_2->x = -1;
                if (find_trap_pos(map, tmp_aim_2, Saken->y, Saken->x, my_abs(map[Saken->y][Saken->x].tunnel) + 1, h, w, &anti_timeout_2) == 0)
                {
                    fprintf(stderr, "error in find_trap_pos\n");
                    free(tmp_aim_2);
                    return;
                }

                if (tmp_aim_2->y != -1 && tmp_aim_2->x != -1)
                {
                    if (tmp_aim->y == Deidara->y && tmp_aim->x == Deidara->x &&
                        (map[Deidara->y][Deidara->x].box <= -4 || map[Deidara->y][Deidara->x].box >= 1) &&
                        (map[Deidara->y][Deidara->x].bomb_tick == 0 || map[Deidara->y][Deidara->x].bomb_tick >= 4))
                    {
                        fprintf(stderr, "Case tunnel 11\n");
                        status->tunnel_kill = 1;
                        status->tunnel_wait = 0;
                        tunnel_die_pos->y = tmp_aim_2->y;
                        tunnel_die_pos->x = tmp_aim_2->x;
                        tunnel_kill_pos->y = tmp_aim->y;
                        tunnel_kill_pos->x = tmp_aim->x;
                    }
                    else if (map[tmp_aim->y][tmp_aim->x].path == 1 &&
                        (map[tmp_aim->y][tmp_aim->x].box <= -4 || map[tmp_aim->y][tmp_aim->x].box >= 1) &&
                        (map[tmp_aim->y][tmp_aim->x].bomb_tick == 0 || map[tmp_aim->y][tmp_aim->x].bomb_tick >= 4))
                    {
                        fprintf(stderr, "Case tunnel 2\n");
                        status->tunnel_kill = 1;
                        status->tunnel_wait = 0;
                        tunnel_die_pos->y = tmp_aim_2->y;
                        tunnel_die_pos->x = tmp_aim_2->x;
                        tunnel_kill_pos->y = tmp_aim->y;
                        tunnel_kill_pos->x = tmp_aim->x;
                    }
                    else if (status->farm == 0 && Deidara->f_a >= 1 && Deidara->f_t == 1 &&
                        (map[tmp_aim->y][tmp_aim->x].box <= -4 || map[tmp_aim->y][tmp_aim->x].box >= 1) &&
                        (map[tmp_aim->y][tmp_aim->x].bomb_tick == 0 || map[tmp_aim->y][tmp_aim->x].bomb_tick >= 4))
                    {
                        fprintf(stderr, "Case tunnel 3\n");
                        status->teleport = 1;
                        status->tunnel_kill = 1;
                        status->tunnel_wait = 0;
                        tunnel_die_pos->y = tmp_aim_2->y;
                        tunnel_die_pos->x = tmp_aim_2->x;
                        tunnel_kill_pos->y = tmp_aim->y;
                        tunnel_kill_pos->x = tmp_aim->x;
                    }
                }
                free(tmp_aim_2);
            }
        }
    }
    else if (map[Saken->y][Saken->x].bomb_tick == 5 && tunnels_around(map, Saken, h, w, tmp_aim) == 1)
    {
        struct pos* tmp_aim_2 = malloc(sizeof(struct pos));
        int anti_timeout_2 = 1;
        tmp_aim_2->y = -1;
        tmp_aim_2->x = -1;
        if (find_trap_pos(map, tmp_aim_2, tmp_aim->y, tmp_aim->x, my_abs(map[Saken->y][Saken->x].tunnel) + 1, h, w, &anti_timeout_2) == 0)
        {
            fprintf(stderr, "error in find_trap_pos\n");
            free(tmp_aim_2);
            return;
        }

        if (tmp_aim_2->y != -1 && tmp_aim_2->x != -1)
        {
            if (tmp_aim_2->y == Deidara->y && tmp_aim_2->x == Deidara->x &&
                (map[Deidara->y][Deidara->x].box <= -4 || map[Deidara->y][Deidara->x].box >= 1) &&
                (map[Deidara->y][Deidara->x].bomb_tick == 0 || map[Deidara->y][Deidara->x].bomb_tick >= 4))
            {
                fprintf(stderr, "Case tunnel 12\n");
                status->tunnel_kill = 1;
                status->tunnel_wait = 0;
                tunnel_die_pos->y = tmp_aim->y;
                tunnel_die_pos->x = tmp_aim->x;
                tunnel_kill_pos->y = tmp_aim_2->y;
                tunnel_kill_pos->x = tmp_aim_2->x;
            }
            else if (map[tmp_aim_2->y][tmp_aim_2->x].path == 1 &&
                (map[tmp_aim_2->y][tmp_aim_2->x].box <= -4 || map[tmp_aim_2->y][tmp_aim_2->x].box >= 1) &&
                (map[tmp_aim_2->y][tmp_aim_2->x].bomb_tick == 0 || map[tmp_aim_2->y][tmp_aim_2->x].bomb_tick >= 4))
            {
                fprintf(stderr, "Case tunnel 4\n");
                status->tunnel_kill = 1;
                status->tunnel_wait = 0;
                tunnel_die_pos->y = tmp_aim->y;
                tunnel_die_pos->x = tmp_aim->x;
                tunnel_kill_pos->y = tmp_aim_2->y;
                tunnel_kill_pos->x = tmp_aim_2->x;
            }
            else if (status->farm == 0 && Deidara->f_a >= 1 && Deidara->f_t == 1 &&
                (map[tmp_aim_2->y][tmp_aim_2->x].box <= -4 || map[tmp_aim_2->y][tmp_aim_2->x].box >= 1) &&
                (map[tmp_aim_2->y][tmp_aim_2->x].bomb_tick == 0 || map[tmp_aim_2->y][tmp_aim_2->x].bomb_tick >= 4))
            {
                fprintf(stderr, "Case tunnel 5\n");
                status->teleport = 1;
                status->tunnel_kill = 1;
                status->tunnel_wait = 0;
                tunnel_die_pos->y = tmp_aim->y;
                tunnel_die_pos->x = tmp_aim->x;
                tunnel_kill_pos->y = tmp_aim_2->y;
                tunnel_kill_pos->x = tmp_aim_2->x;
            }
        }

        anti_timeout_2 = 1;
        tmp_aim_2->y = -1;
        tmp_aim_2->x = -1;
        if (find_trap_pos(map, tmp_aim_2, tmp_aim->y, tmp_aim->x, my_abs(map[Saken->y][Saken->x].tunnel) + 2, h, w, &anti_timeout_2) == 0)
        {
            fprintf(stderr, "error in find_trap_pos\n");
            free(tmp_aim_2);
            return;
        }

        if (tmp_aim_2->y != -1 && tmp_aim_2->x != -1)
        {
            if (tmp_aim_2->y == Deidara->y && tmp_aim_2->x == Deidara->x &&
                (map[Deidara->y][Deidara->x].box <= -4 || map[Deidara->y][Deidara->x].box >= 1) &&
                (map[Deidara->y][Deidara->x].bomb_tick == 0 || map[Deidara->y][Deidara->x].bomb_tick >= 4))
            {
                fprintf(stderr, "Case tunnel 12\n");
                status->tunnel_kill = 1;
                status->tunnel_wait = 0;
                tunnel_die_pos->y = tmp_aim->y;
                tunnel_die_pos->x = tmp_aim->x;
                tunnel_kill_pos->y = tmp_aim_2->y;
                tunnel_kill_pos->x = tmp_aim_2->x;
            }
            if (map[tmp_aim_2->y][tmp_aim_2->x].path == 1 &&
                (map[tmp_aim_2->y][tmp_aim_2->x].box <= -4 || map[tmp_aim_2->y][tmp_aim_2->x].box >= 1) &&
                (map[tmp_aim_2->y][tmp_aim_2->x].bomb_tick == 0 || map[tmp_aim_2->y][tmp_aim_2->x].bomb_tick >= 4))
            {
                fprintf(stderr, "Case tunnel 6\n");
                status->tunnel_kill = 1;
                status->tunnel_wait = 0;
                tunnel_die_pos->y = tmp_aim->y;
                tunnel_die_pos->x = tmp_aim->x;
                tunnel_kill_pos->y = tmp_aim_2->y;
                tunnel_kill_pos->x = tmp_aim_2->x;
            }
        }
        free(tmp_aim_2);
    }
    else if (my_abs(map[Saken->y][Saken->x].tunnel) <= 2)
    {
        anti_timeout = 1;
        tmp_aim->y = -1;
        tmp_aim->x = -1;
        if (find_trap_pos(map, tmp_aim, Saken->y, Saken->x, my_abs(map[Saken->y][Saken->x].tunnel) + 1, h, w, &anti_timeout) == 0)
        {
            fprintf(stderr, "error in find_trap_pos\n");
            free(tmp_aim);
            return;
        }

        if (tmp_aim->y != -1 && tmp_aim->x != -1)
        {
            if (tmp_aim->y == Deidara->y && tmp_aim->x == Deidara->x &&
                (map[Deidara->y][Deidara->x].box <= -4 || map[Deidara->y][Deidara->x].box >= 1) &&
                (map[Deidara->y][Deidara->x].bomb_tick == 0 || map[Deidara->y][Deidara->x].bomb_tick >= 4))
            {
                fprintf(stderr, "Case tunnel 7\n");
                status->tunnel_kill = 1;
                status->tunnel_wait = 0;
                tunnel_die_pos->y = Saken->y;
                tunnel_die_pos->x = Saken->x;
                tunnel_kill_pos->y = tmp_aim->y;
                tunnel_kill_pos->x = tmp_aim->x;
            }
        }
    }
    else if (my_abs(map[enemy_aim->y][enemy_aim->x].enemy_tunnel) <= 3)
    {
        if (map[enemy_aim->y][enemy_aim->x].enemy_path <= 2)
        {
            if (map[enemy_aim->y][enemy_aim->x].enemy_path < map[enemy_aim->y][enemy_aim->x].path)
            {
                anti_timeout = 1;
                tmp_aim->y = -1;
                tmp_aim->x = -1;
                if (find_trap_pos(map, tmp_aim, enemy_aim->y, enemy_aim->x, my_abs(map[enemy_aim->y][enemy_aim->x].tunnel) + 2, h, w, &anti_timeout) == 0)
                {
                    fprintf(stderr, "error in find_trap_pos\n");
                    free(tmp_aim);
                    return;
                }

                if (tmp_aim->y != -1 && tmp_aim->x != -1)
                {
                    if (my_abs_dif(tmp_aim->y, enemy_aim->y) + my_abs_dif(tmp_aim->x, enemy_aim->x) == 1)
                    {
                        if (map[tmp_aim->y][tmp_aim->x].path == map[enemy_aim->y][enemy_aim->x].enemy_path)
                        {
                            fprintf(stderr, "Case tunnel 8\n");
                            status->tunnel_kill = 1;
                            status->tunnel_wait = 0;
                            tunnel_die_pos->y = enemy_aim->y;
                            tunnel_die_pos->x = enemy_aim->x;
                            tunnel_kill_pos->y = tmp_aim->y;
                            tunnel_kill_pos->x = tmp_aim->x;
                        }
                    }
                    if (my_abs_dif(tmp_aim->y, enemy_aim->y) + my_abs_dif(tmp_aim->x, enemy_aim->x) == 2)
                    {
                        if (map[tmp_aim->y][tmp_aim->x].path == map[enemy_aim->y][enemy_aim->x].enemy_path + 1 ||
                            map[tmp_aim->y][tmp_aim->x].path == map[enemy_aim->y][enemy_aim->x].enemy_path)
                        {
                            fprintf(stderr, "Case tunnel 9\n");
                            status->tunnel_kill = 1;
                            status->tunnel_wait = 0;
                            tunnel_die_pos->y = enemy_aim->y;
                            tunnel_die_pos->x = enemy_aim->x;
                            tunnel_kill_pos->y = tmp_aim->y;
                            tunnel_kill_pos->x = tmp_aim->x;
                        }
                    }
                }
            }
        }
    }
    free(tmp_aim);
}

int tunnels_around(struct cell** map, struct bot* Saken, int h, int w, struct pos* tmp_pos)
{
    int number = 0;
    int y = Saken->y;
    int x = Saken->x;
    if (y - 1 >= 0 && map[y - 1][x].tunnel != 0)
    {
        number++;
        tmp_pos->y = y - 1;
        tmp_pos->x = x;
    }
    if (y + 1 < h && map[y + 1][x].tunnel != 0)
    {
        number++;
        tmp_pos->y = y + 1;
        tmp_pos->x = x;
    }
    if (x - 1 >= 0 && map[y][x - 1].tunnel != 0)
    {
        number++;
        tmp_pos->y = y;
        tmp_pos->x = x - 1;
    }
    if (x + 1 < w && map[y][x + 1].tunnel != 0)
    {
        number++;
        tmp_pos->y = y;
        tmp_pos->x = x + 1;
    }
    return number;
}

int find_trap_pos(struct cell** map, struct pos* tmp_aim, int y, int x, int fin_value, int h, int w, int* anti_timeout)
{
    (*anti_timeout)++;
    if (*anti_timeout == 100)
    {
        return 0;
    }

    if (y - 1 >= 0)
    {
        if ((my_abs(map[y - 1][x].enemy_tunnel) == fin_value || my_abs(map[y - 1][x].enemy_tunnel) > 100) && map[y - 1][x].bomb_radius == 0)
        {
            tmp_aim->y = y - 1;
            tmp_aim->x = x;
            return 1;
        }
        else if (my_abs(map[y - 1][x].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1 && map[y - 1][x].bomb_radius == 0)
        {
            find_trap_pos(map, tmp_aim, y - 1, x, fin_value, h, w, anti_timeout);
        }
    }
    if (y + 1 < h)
    {
        if ((my_abs(map[y + 1][x].enemy_tunnel) == fin_value || my_abs(map[y + 1][x].enemy_tunnel) > 100) && map[y + 1][x].bomb_radius == 0)
        {
            tmp_aim->y = y + 1;
            tmp_aim->x = x;
            return 1;
        }
        else if (my_abs(map[y + 1][x].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1 && map[y + 1][x].bomb_radius == 0)
        {
            find_trap_pos(map, tmp_aim, y + 1, x, fin_value, h, w, anti_timeout);
        }
    }
    if (x - 1 >= 0)
    {
        if ((my_abs(map[y][x - 1].enemy_tunnel) == fin_value || my_abs(map[y][x - 1].enemy_tunnel) > 100) && map[y][x - 1].bomb_radius == 0)
        {
            tmp_aim->y = y;
            tmp_aim->x = x - 1;
            return 1;
        }
        else if (my_abs(map[y][x - 1].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1 && map[y][x - 1].bomb_radius == 0)
        {
            find_trap_pos(map, tmp_aim, y, x - 1, fin_value, h, w, anti_timeout);
        }
    }
    if (x + 1 < w)
    {
        if ((my_abs(map[y][x + 1].enemy_tunnel) == fin_value || my_abs(map[y][x + 1].enemy_tunnel) > 100) && map[y][x + 1].bomb_radius == 0)
        {
            tmp_aim->y = y;
            tmp_aim->x = x + 1;
            return 1;
        }
        else if (my_abs(map[y][x + 1].enemy_tunnel) == my_abs(map[y][x].enemy_tunnel) + 1 && map[y][x + 1].bomb_radius == 0)
        {
            find_trap_pos(map, tmp_aim, y, x + 1, fin_value, h, w, anti_timeout);
        }
    }
    return 1;
}



int check_potential(struct cell** map, struct bot* Saken, int h, int w, struct pos* art_pos)
{
    int y = Saken->y;
    int x = Saken->x;
    if (y - 1 >= 0)
    {
        if (map[y - 1][x].box != 50 && (map[y - 1][x].box == 1 || map[y - 1][x].box < 0))
        {
            if (check_around(map, y - 1, x, h, w) == 1)
            {
                art_pos->y = y - 1;
                art_pos->x = x;
                return 1;
            }
        }
    }
    if (y + 1 < h)
    {
        if (map[y + 1][x].box != 50 && (map[y + 1][x].box == 1 || map[y + 1][x].box < 0))
        {
            if (check_around(map, y + 1, x, h, w) == 1)
            {
                art_pos->y = y + 1;
                art_pos->x = x;
                if (art_pos->y != -1 && art_pos->x != -1)
                {
                    return 1;
                }
            }
        }
    }
    if (x - 1 >= 0)
    {
        if (map[y][x - 1].box != 50 && (map[y][x - 1].box == 1 || map[y][x - 1].box < 0))
        {
            if (check_around(map, y, x - 1, h, w) == 1)
            {
                art_pos->y = y;
                art_pos->x = x - 1;
                if (art_pos->y != -1 && art_pos->x != -1)
                {
                    return 1;
                }
            }
        }
    }
    if (x + 1 < w)
    {
        if (map[y][x + 1].box != 50 && (map[y][x + 1].box == 1 || map[y][x + 1].box < 0))
        {
            if (check_around(map, y, x + 1, h, w) == 1)
            {
                art_pos->y = y;
                art_pos->x = x + 1;
                if (art_pos->y != -1 && art_pos->x != -1)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int check_around(struct cell** map, int y, int x, int h, int w)
{
    if (y - 1 >= 0)
    {
        if (map[y - 1][x].box <= 1)
        {
            if (map[y - 1][x].surround < 2)
            {
                return 0;
            }
        }
    }
    if (y + 1 < h)
    {
        if (map[y + 1][x].box <= 1)
        {
            if (map[y + 1][x].surround < 2)
            {
                return 0;
            }
        }
    }
    if (x - 1 >= 0)
    {
        if (map[y][x - 1].box <= 1)
        {
            if (map[y][x - 1].surround < 2)
            {
                return 0;
            }
        }
    }
    if (x + 1 < w)
    {
        if (map[y][x + 1].box <= 1)
        {
            if (map[y][x + 1].surround < 2)
            {
                return 0;
            }
        }
    }
    return 1;
}



void find_dir(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* aim, int h, int w, int* dir, int current_number, struct situation* status, struct pos* enemy_aim)
{
    int anti_timeout = 0;
    int y = aim->y;
    int x = aim->x;
    int wait = 0;
    struct pos next = { -1, -1 };

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
            *dir = 5;

            fprintf(stderr, "ALERT ALERT ALERT ALERT Find_dir timeout.\n");
            /*if (x - 1 >= 0) //ACTIVATE FOR ARENA
                if (map[y][x - 1].enemy_box_2 <= 1)
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

    /*
    Get out with TP or Jump if can't get out properly
    */
    if ((map[Deidara->y][Deidara->x].box == -1 || map[Deidara->y][Deidara->x].bomb_tick == 1) && ((next.y != -1 && map[next.y][next.x].box == -1) || next.y == -1))
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
            else
            {
                if (safe_suicide(map, Deidara, h, w) != -1)
                {
                    fprintf(stderr, "self suicide\n");
                    *dir = safe_suicide(map, Deidara, h, w);
                }
                else
                {
                    fprintf(stderr, "WE GOT TRAPPED\n");
                    *dir = 4;
                }
            }
        }
        fprintf(stderr, "new dir is %d\n", *dir);
        return;
    }


    /*
    Normal escape mech
    */
    if (wait != 0)
    {
        fprintf(stderr, "escape?\n");
        fprintf(stderr, "wait %d dir %d next %d %d %d\n", wait, *dir, next.y, next.x, map[next.y][next.x].box);
        if (map[Deidara->y][Deidara->x].box == -2 && map[next.y][next.x].box == -2)
        {
            fprintf(stderr, "escape second if\n");
            get_out(map, Deidara, dir, h, w, -2);
            fprintf(stderr, "new dir is %d\n", *dir);
            return;
        }
        else if (((map[Deidara->y][Deidara->x].box >= 1 && map[Deidara->y][Deidara->x].bomb_tick != wait) || map[Deidara->y][Deidara->x].box < -1) && wait == -1 * map[next.y][next.x].box)
        {
            *dir = 5;
        }
    }

    /*
    Bomb plant case
    */
    if (aim->y == Deidara->y && aim->x == Deidara->x && current_number != 0 && map[aim->y][aim->x].goal > -100)
    {
        fprintf(stderr, "Bomb plant case 1 %d %d \n", y, x);
        *dir = 4;
        return;
    }
    else if ((map[aim->y][aim->x].path >= 5 && map[aim->y][aim->x].path > Deidara->f_r) && map[Deidara->y][Deidara->x].goal >= value && Deidara->f_a > 0)
    {
        fprintf(stderr, "Bomb plant case 2 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if ((map[aim->y][aim->x].path > Deidara->f_r || (Deidara->y != aim->y && Deidara->x != aim->x)) &&
        map[Deidara->y][Deidara->x].goal >= value * 2 && Deidara->f_a > 0)
    {
        fprintf(stderr, "Bomb plant case 3 %d %d %d\n", aim->y, aim->x, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if ((map[aim->y][aim->x].path > Deidara->f_r || (Deidara->y != aim->y && Deidara->x != aim->x)) &&
        map[Deidara->y][Deidara->x].goal >= value && Deidara->a_a > 1 && Deidara->f_a > 0)
    {
        fprintf(stderr, "Bomb plant case 4 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if ((map[aim->y][aim->x].path > Deidara->f_r || (Deidara->y != aim->y && Deidara->x != aim->x)) &&
        Deidara->f_r == 2 && map[Deidara->y][Deidara->x].box == -4 && map[Deidara->y][Deidara->x].goal >= value && Deidara->f_a > 0)
    {
        *dir = 4;
        fprintf(stderr, "Bomb plant case 5 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
    }
    else if (map[aim->y][aim->x].path >= 5 && map[aim->y][aim->x].path > Deidara->f_r && map[Deidara->y][Deidara->x].extragoal > 0 && Deidara->f_a > 0 && map[Deidara->y][Deidara->x].bomb_tick == 0)
    {
        fprintf(stderr, "Bomb plant case 6 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if ((map[aim->y][aim->x].path > Deidara->f_r || (Deidara->y != aim->y && Deidara->x != aim->x)) &&
        map[Deidara->y][Deidara->x].extragoal > 0 && map[Deidara->y][Deidara->x].bomb_tick == 0 && Deidara->f_a > 0)
    {
        fprintf(stderr, "Bomb plant case 7 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }
    else if (Deidara->f_a > 0 && Deidara->y == enemy_aim->y && Deidara->x == enemy_aim->x && map[Deidara->y][Deidara->x].bomb_tick == 0 && map[Deidara->y][Deidara->x].box == 1)
    {
        fprintf(stderr, "Bomb plant case 8 %d %d %d %d\n", aim->y, aim->x, Deidara->f_a, Deidara->f_r);
        *dir = 4;
        return;
    }
}

void get_tp(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir, int current_number)
{
    int anti_timeout = 0;
    int y = aim->y;
    int x = aim->x;
    int wait = 0;
    struct pos next = { -1, -1 };

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
            *dir = 5;

            fprintf(stderr, "ALERT ALERT ALERT ALERT Find_dir timeout.\n");
            /*if (x - 1 >= 0) //ACTIVATE FOR ARENA
                if (map[y][x - 1].enemy_box_2 <= 1)
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

    /*
    Get out with TP or Jump if can't get out properly
    */
    if ((map[Deidara->y][Deidara->x].box == -1 || map[Deidara->y][Deidara->x].bomb_tick == 1) && ((next.y != -1 && map[next.y][next.x].box == -1) || next.y == -1))
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
            else if (safe_suicide(map, Deidara, h, w) != -1)
            {
                *dir = safe_suicide(map, Deidara, h, w);
            }
            else
            {
                fprintf(stderr, "WE GOT TRAPPED\n");
                *dir = 4;
            }
        }
        fprintf(stderr, "new dir is %d\n", *dir);
        return;
    }


    /*
    Normal escape mech
    */
    if (wait != 0)
    {
        fprintf(stderr, "escape?\n");
        fprintf(stderr, "wait %d dir %d next %d %d %d\n", wait, *dir, next.y, next.x, map[next.y][next.x].box);
        if (map[Deidara->y][Deidara->x].box == -2 && map[next.y][next.x].box == -2)
        {
            fprintf(stderr, "escape second if\n");
            get_out(map, Deidara, dir, h, w, -2);
            fprintf(stderr, "new dir is %d\n", *dir);
            return;
        }
        else if (((map[Deidara->y][Deidara->x].box >= 1 && map[Deidara->y][Deidara->x].bomb_tick != wait) || map[Deidara->y][Deidara->x].box < -1) && wait == -1 * map[next.y][next.x].box)
        {
            *dir = 5;
        }
    }
}

int tunnel_kill_acquired(struct cell** map, struct bot* Deidara, struct bot* Saken, struct pos* tunnel_kill_pos, struct pos* tunnel_die_pos, int h, int w, int* dir, struct situation* status)
{
    int anti_timeout = 0;
    int y = tunnel_kill_pos->y;
    int x = tunnel_kill_pos->x;
    int wait = 0;
    struct pos next = { -1, -1 };

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
            }
        }
        if (*dir == 1) x--;
        if (*dir == 3) y--;
        if (*dir == 0) x++;
        if (*dir == 2) y++;
        if (anti_timeout < 50) anti_timeout++;
        else if (anti_timeout >= 50)
        {
            fprintf(stderr, "Find_dir timeout.\n");
            *dir = 5;
            status->tunnel_kill = 0;
            return 1;
        }
    }

    /*
    Escape with jump or tp if can't get out properly
    */
    if (((map[Deidara->y][Deidara->x].box == -1 || map[Deidara->y][Deidara->x].bomb_tick == 1) && next.y != -1 && map[next.y][next.x].box == -1) ||
        ((map[Deidara->y][Deidara->x].box == -1 || map[Deidara->y][Deidara->x].bomb_tick == 1) && map[tunnel_kill_pos->y][tunnel_kill_pos->x].box == -1))
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
            else if (safe_suicide(map, Deidara, h, w) != -1)
            {
                *dir = safe_suicide(map, Deidara, h, w);
            }
            else
            {
                fprintf(stderr, "WE GOT TRAPPED\n");
                *dir = 4;
            }
        }
        fprintf(stderr, "new dir is %d\n", *dir);
        status->tunnel_kill = 0;
        return 0;
    }

    /*
    Normal escape
    */
    if (wait != 0)
    {
        fprintf(stderr, "escape?\n");
        fprintf(stderr, "wait %d dir %d next %d %d %d\n", wait, *dir, next.y, next.x, map[next.y][next.x].box);

        if (map[Deidara->y][Deidara->x].box == -2 && map[next.y][next.x].box == -2)
        {
            fprintf(stderr, "escape second if\n");
            get_out(map, Deidara, dir, h, w, -2);
            fprintf(stderr, "new dir is %d\n", *dir);
            status->tunnel_kill = 0;
            return 0;
        }
        else if ((map[Deidara->y][Deidara->x].box >= 1 || map[Deidara->y][Deidara->x].box < -1) && wait == -1 * map[next.y][next.x].box)
        {
            *dir = 5;
            status->tunnel_kill = 0;
        }
    }

    /*
    Bomb plant case
    */
    if (Deidara->y == tunnel_kill_pos->y && Deidara->x == tunnel_kill_pos->x)
    {
        if ((Saken->y == tunnel_die_pos->y && Saken->x == tunnel_die_pos->x) || status->tunnel_wait == 2 && map[Deidara->y][Deidara->x].bomb_tick == 0)
        {
            *dir = 4;
            status->tunnel_kill = 0;
        }
        else if (my_abs_dif(Saken->y, tunnel_die_pos->y) + my_abs_dif(Saken->x, tunnel_die_pos->x) == 1)
        {
            *dir = 5;
            if (Saken->box_count >= Deidara->box_count)
            {
                status->tunnel_wait++;
            }
        }
        else if (my_abs_dif(Saken->y, tunnel_die_pos->y) + my_abs_dif(Saken->x, tunnel_die_pos->x) >= 2)
        {
            status->tunnel_kill = 0;
            return 1;
        }
        else
        {
            status->tunnel_kill = 0;
            return 1;
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

int safe_suicide(struct cell** map, struct bot* Deidara, int h, int w)
{
    int x = Deidara->x;
    int y = Deidara->y;
    fprintf(stderr, "ss\n");
    if (x - 1 >= 0)
    {
        fprintf(stderr, "ss1\n");
        if ((map[y][x - 1].box == 1 || (map[y][x - 1].box < 0 && map[y][x - 1].bomb_tick == 0)) && map[y][x - 1].bomb_Saken == 0)
        {
            return 0;
        }
    }
    if (x + 1 < w)
    {
        fprintf(stderr, "ss2\n");
        if ((map[y][x + 1].box == 1 || (map[y][x + 1].box < 1 && map[y][x + 1].bomb_tick == 0)) && map[y][x + 1].bomb_Saken == 0)
        {
            return 1;
        }
    }
    if (y - 1 >= 0)
    {
        fprintf(stderr, "ss3\n");
        if ((map[y - 1][x].box == 1 || (map[y - 1][x].box < 1 && map[y - 1][x].bomb_tick == 0)) && map[y - 1][x].bomb_Saken == 0)
        {
            return 2;
        }
    }
    if (y + 1 < h)
    {
        fprintf(stderr, "ss4\n");
        if ((map[y + 1][x].box == 1 || (map[y + 1][x].box < 0 && map[y + 1][x].bomb_tick == 0)) && map[y + 1][x].bomb_Saken == 0)
        {
            return 3;
        }
    }
    return -1;
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
            fprintf(stderr, "p"); //path
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
        char tmp2[200];
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "Final enemy goal: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].enemy_goal);
            }
            fprintf(stderr, "%s \n", tmp2);
        }
        fprintf(stderr, "\n");
        break;
    case 8:
        //fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "F"); //Final map
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].goal);
            }
            fprintf(stderr, "\n");
        }
        //fprintf(stderr, "\n");
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
    case 11:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "bomb radius: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].bomb_radius);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 12:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "bomb tick: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].bomb_tick);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 13:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "surround: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].surround);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 14:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "wood box: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].wood_box);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 15:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "extragoal: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].extragoal);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 16:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "bomb_Saken: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%2d ", map[i][j].bomb_Saken);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
    default:
        break;
    }
}

void print_map_2(struct cell** map, int h, int w, int map_index)
{
    switch (map_index)
    {
    case 0:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "enemy_box_2: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%4d ", map[i][j].enemy_box_2);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 1:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "enemy_goal_2: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%4d ", map[i][j].enemy_goal_2);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 2:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "enemy_path_2: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%4d ", map[i][j].enemy_path_2);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 3:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "Enemy tunnel 2: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%4d ", map[i][j].enemy_tunnel_2);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 4:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "enemy_adjust_goal_2: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].enemy_goal_2);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    case 5:
        fprintf(stderr, "\n");
        for (int i = 0; i < h; i++)
        {
            fprintf(stderr, "finam enemy goal 2: ");
            for (int j = 0; j < w; j++)
            {
                fprintf(stderr, "%5d ", map[i][j].enemy_goal_2);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        break;
    default:
        break;
    }
}

void freentf(struct cell** map, int h, int w, struct pos* aim, struct pos* enemy_aim, int* sector_count, struct pos* tp_aim)
{
    //free part
    for (int i = 0; i < w; i++)
    {
        free(map[i]);
    }
    free(enemy_aim);
    free(aim);
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