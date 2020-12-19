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
    status->assassin = 0;
    status->trapped = 0;
    status->teleport = 0;
    status->assassin_wait = 0;
    status->tunnel_wait = 0;
    status->bad_next_predict = 1;
    status->tunnel_kill = 0;
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
        //fprintf(stderr, "h - %d, w - %d, player_id - %d, tick - %d\n", h, w, player_id, tick);

        if (status->assassin == 1)
        {
            fprintf(stderr, "FOUND A CASE TO KILL\n");
        }
        if (status->tunnel_kill == 1)
        {
            fprintf(stderr, "FOUND A CASE TO TUNNEL KILL\n");
        }
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

        /*
        Number of entities
        */
        int n;
        scanf("%d", &n);
        getchar();
        //fprintf(stderr, "Number of entities - %d\n", n);

        /*
        Read entities
        */
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
        //fprintf(stderr, "After fill_entity\n");
        //print_map(map, h, w, 0);
        //print_map(map, h, w, 11);
        //print_map(map, h, w, 12);

        /*
        Adjust bomb goal
        */
        adjust_bomb_goal(map, Deidara, h, w);

        /*
        Count box points
        */
        Deidara->future_box_count = 0;
        Saken->future_box_count = 0;
        count_box_points(map, Deidara, Saken, player_id, h, w);
        //print_map(map, h, w, 14);
        //fprintf(stderr, "Deidara count box = %d, Saken count box = %d.\n", Deidara->box_count, Saken->box_count);
        //fprintf(stderr, "Deidara future count box = %d, Saken future count box = %d.\n", Deidara->future_box_count, Saken->future_box_count);

        /*
        Read features
        */
        Deidara->f_t = 0;
        Deidara->f_j = 0;
        Saken->f_t = 0;
        Saken->f_j = 0;
        int f_n = 0;
        scanf("%d", &f_n);
        //fprintf(stderr, "Number of features - %d\n", f_n);
        getchar();
        for (int i = 0; i < f_n; i++) {
            int p_id, type;
            scanf("%d%d", &p_id, &type);
            read_features(map, Deidara, Saken, player_id, p_id, type);
            //fprintf(stderr, "Player id is %d Type is %d\n", p_id, type);
        }
        //fprintf(stderr, "Deidara teleport %d jump %d\n", Deidara->f_t, Deidara->f_j);
        //fprintf(stderr, "Saken teleport %d jump %d\n", Saken->f_t, Saken->f_j);
        if (Saken->f_j == 1)
        {
            status->farm == 1;
        }
        /*
        Late game record
        *//*
        if (tick >= 200 && Saken->box_count > Deidara->box_count)
        {
            if (late_number <= 50)
            {
                late_sequence[late_number]->y = Saken->y;
                late_sequence[late_number]->x = Saken->x;
                late_number++;
            }
            else
            {
                find_sequence(map, Deidara, Saken, late_sequence);
            }
        }
        */

        /*
        Fill path
        */
        //fprintf(stderr, "Before fill path\n");
        fill_path(map, Deidara, Saken, h, w);
        //fprintf(stderr, "After fill path\n");
        //print_map(map, h, w, 2);

        /*
        Feature goal values
        */
        add_feature_goal(map, Deidara, Saken, h, w, status, coef);
        //print_map(map, h, w, 4);

        /*
        Fill goal
        */
        //fprintf(stderr, "Before fill_goal\n");
        fill_goal(map, Deidara, h, w, status);
        //fprintf(stderr, "After fill_goal\n");

        /*
        fill_enemy_goal
        */
        if (Saken->x != -1)
        {
            //fprintf(stderr, "Before fill enemy goal\n");
            fill_enemy_goal(map, Saken, h, w);
            //fprintf(stderr, "After fill enemy goal\n");
            //print_map(map, h, w, 5);
        }


        /*
        Fill positive goal if tick > 4
        */
        //fprintf(stderr, "Before fill_extragoal\n");
        fill_extragoal(map, Deidara, player_id, h, w);
        // print_map(map, h, w, 15);
        //fprintf(stderr, "Before fill_extragoal\n");

        /*
        Fill_features
        */
        //fprintf(stderr, "Before fill features\n");
        fill_features(map, Deidara, h, w);
        //fprintf(stderr, "After fill features\n");
        //print_map(map, h, w, 9);

        /*
        Fill tunnel
            and
        Fill enemy path
            and
        Fill enemy tunnel
        */
        if (Saken->x != -1)
        {
            //fprintf(stderr, "Before fill tunnel\n");
            fill_tunnel(map, Saken, h, w);
            //fprintf(stderr, "After fill tunnel\n");
            //print_map(map, h, w, 1);
            //fprintf(stderr, "Before fill enemy path\n");
            fill_enemy_path(map, Deidara, Saken, h, w);
            //fprintf(stderr, "After fill enemy path\n");
            //print_map(map, h, w, 6);
            //fprintf(stderr, "Before fill enemy tunnel\n");
            fill_enemy_tunnel(map, Saken, h, w);
            //fprintf(stderr, "After fill enemy tunnel\n");
            //print_map(map, h, w, 3);
        }

        /*
        Change goals if dangerous tunnel
        */
        if (Saken->x != -1)
        {
            //fprintf(stderr, "Before anti_trap\n");
            anti_trap(map, Deidara, Saken, h, w);
            //fprintf(stderr, "After anti_trap\n");
            //print_map(map, h, w, 10);
        }

        /*
        Final enemy map and adjust enemy goal
        */
        struct pos* enemy_aim = malloc(sizeof(struct pos));
        struct pos* enemy_aim_1 = malloc(sizeof(struct pos));
        if (Saken->x != -1)
        {
            enemy_aim->y = Saken->y;
            enemy_aim->x = Saken->x;
            enemy_aim_1->y = Saken->y;
            enemy_aim_1->x = Saken->x;
            //fprintf(stderr, "Before adjust enemy goal\n");
            //adjust_enemy_goal(map, h, w);
            //fprintf(stderr, "After adjust enemy goal\n");
            //fprintf(stderr, "Before fill final enemy map\n");
            final_enemy_map(map, Saken, h, w, enemy_aim, enemy_aim_1);
            //fprintf(stderr, "After fill final enemy map\n");
            //print_map(map, h, w, 7);
            //fprintf(stderr, "Before adjust_priority\n");
            adjust_priority(map, Deidara, Saken, status, enemy_aim, h, w);
            //fprintf(stderr, "After adjust priority\n");
        }


        /*
        Enemy second goal
        */
        struct pos* enemy_aim_2_1 = malloc(sizeof(struct pos));
        struct pos* enemy_aim_2_2 = malloc(sizeof(struct pos));
        if (Saken->x != -1)
        {
            enemy_aim_2_1->y = enemy_aim->y;
            enemy_aim_2_1->x = enemy_aim->x;
            enemy_aim_2_2->y = enemy_aim->y;
            enemy_aim_2_2->x = enemy_aim->x;
            //fprintf(stderr, "Before fill enemy box 2\n");
            fill_enemy_box_2(map, Saken, enemy_aim, h, w);
            //fprintf(stderr, "After fill enemy box 2\n");
            //print_map_2(map, h, w, 0);
            //fprintf(stderr, "Before fill enemy goal 2\n");
            fill_enemy_goal_2(map, Saken, h, w);
            //fprintf(stderr, "After fill enemy goal 2\n");
            //print_map_2(map, h, w, 1);
            //fprintf(stderr, "Before fill enemy path 2\n");
            fill_enemy_path_2(map, enemy_aim, h, w);
            //fprintf(stderr, "After fill enemy path 2\n");
            //print_map_2(map, h, w, 2);
            //fprintf(stderr, "Before fill enemy tunnel 2\n");
            fill_enemy_tunnel_2(map, Saken, h, w);
            //fprintf(stderr, "After fill enemy tunnel 2\n");
            //print_map_2(map, h, w, 3);
            //fprintf(stderr, "Before adjust enemy goal 2\n");
            //adjust_enemy_goal_2(map, h, w);
            //fprintf(stderr, "After adjust enemy goal 2\n");
            //print_map_2(map, h, w, 4);
            //fprintf(stderr, "Before fill final enemy map 2\n");
            final_enemy_map_2(map, enemy_aim, h, w, enemy_aim_2_1, enemy_aim_2_2);
            //fprintf(stderr, "After fill final enemy map 2\n");
            //print_map_2(map, h, w, 5);
        }

        //fprintf(stderr, "Before fill surround\n");
        fill_surround(map, h, w);
        //fprintf(stderr, "After fill surround\n");
        //print_map(map, h, w, 13);

        /*
        Teleport escape options
        */
        struct pos* tp_aim = malloc(sizeof(struct pos)); // TEMPORARY SOLUTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        struct pos* escape_pos1 = malloc(sizeof(struct pos));
        escape_teleport(map, Deidara, Saken, h, w, escape_pos1);
        if (status->tp_on_map == 1)
        {
            tp_aim->y = tp_on_map_pos->y;
            tp_aim->x = tp_on_map_pos->x;
        }
        else
        {
            tp_aim->y = escape_pos1->y;
            tp_aim->x = escape_pos1->x;
        }

        /*
        Fill final goal
        */
        int dir = 5;
        struct pos* aim = malloc(sizeof(struct pos));
        aim->y = Deidara->y;
        aim->x = Deidara->x;
        //fprintf(stderr, "Min tick %d\n", min_tick);
        //fprintf(stderr, "Before final map\n");
        final_map(map, Deidara, Saken, h, w, aim, enemy_aim, &min_tick);
        //fprintf(stderr, "After final map\n");
        print_map(map, h, w, 8);

        /*
        Tunnel kill case
        */
        struct pos* enemy_next = malloc(sizeof(struct pos));
        status->bad_next_predict = 1;
        if (Saken->x != -1 && status->tunnel_kill == 0)
        {
            enemy_next->y = -1;
            enemy_next->x = -1;
            //fprintf(stderr, "Before find enemy next\n");
            status->bad_next_predict = find_enemy_next(map, Saken, h, w, enemy_aim, enemy_aim_1, enemy_next);
            //fprintf(stderr, "After find enemy next\n");
            fprintf(stderr, "Enemy next is %d %d\n", enemy_next->y, enemy_next->x);
            if (status->bad_next_predict == 0 && (map[enemy_next->y][enemy_next->x].surround == 3 || map[Saken->y][Saken->x].surround == 3))
            {
                //fprintf(stderr, "Before find tunnel kill\n");
                find_tunnel_kill(map, Deidara, Saken, enemy_next, status, h, w, tunnel_kill_pos, tunnel_die_pos);
                //fprintf(stderr, "After find tunnel kill\n");
            }
        }

        /*
        find trap pos for assassin
        */
        if (Saken->x != -1)
        {
            int fork;
            if (status->assassin == 0)
            {
                fork = find_enemy_dir(map, Deidara, enemy_aim, enemy_aim_2_1, h, w, trap_pos, die_pos);
                int aim_dif_1 = map[enemy_aim->y][enemy_aim->x].enemy_goal - map[enemy_aim_1->y][enemy_aim_1->x].enemy_goal;
                int aim_dif_2 = map[enemy_aim_2_1->y][enemy_aim_2_1->x].enemy_goal_2 - map[enemy_aim_2_2->y][enemy_aim_2_2->x].enemy_goal_2;
                //fprintf(stderr, "aim dif 1 - %d aim dif 2 - %d \n", aim_dif_1, aim_dif_2);
                if (fork == 0 && aim_dif_1 >= 1 && aim_dif_2 >= 1 && map[enemy_aim->y][enemy_aim->x].enemy_path <= 1)
                {
                    if ((map[trap_pos->y][trap_pos->x].path == map[enemy_aim->y][enemy_aim->x].enemy_path + 1 ||
                        map[trap_pos->y][trap_pos->x].path == map[enemy_aim->y][enemy_aim->x].enemy_path) &&
                        map[die_pos->y][die_pos->x].surround == 2)
                    {
                        status->assassin = 1;
                        status->assassin_wait = 0;
                        decoy_pos->y = enemy_aim->y;
                        decoy_pos->x = enemy_aim->x;
                        fprintf(stderr, "FOUND A CASE TO ASSASSIN\n");
                    }
                }
            }
            else if (status->assassin == 1)
            {
                if (decoy_pos->y != enemy_aim->y || decoy_pos->x != enemy_aim->x)
                {
                    if (my_abs_dif(Saken->y, die_pos->y) + my_abs_dif(Saken->x, die_pos->x) >= 2)
                    {
                        status->assassin = 0;
                    }
                }
            }
        }

        /*
        Fill the direction
        */
        if (status->teleport == 1)
        {
            if (status->tunnel_kill == 1 && Saken->f_t == 0 && Saken->f_j == 0)
            {
                dir = 7;
                tp_aim->y = tunnel_kill_pos->y;
                tp_aim->x = tunnel_kill_pos->x;
                status->assassin = 0;
            }
            else if (status->tp_on_map == 1)
            {
                dir = 7;
                tp_aim->y = tp_on_map_pos->y;
                tp_aim->x = tp_on_map_pos->x;
                status->tp_on_map = 0;
                status->tunnel_kill = 0;
                status->assassin = 0;
            }
            else if (status->tunnel_kill == 1)
            {
                dir = 7;
                tp_aim->y = tunnel_kill_pos->y;
                tp_aim->x = tunnel_kill_pos->x;
                status->assassin = 0;
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
                find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status);
            }
            status->assassin = 0;
        }
        else if (status->assassin == 1 && Saken->f_t == 0 && Saken->f_j == 0) ////????????????????????????????????????????????????????????????????????????????
        {
            fprintf(stderr, "Final aim to assassin is %d %d %d Enemy %d %d\n", map[trap_pos->y][trap_pos->x].goal, trap_pos->y, trap_pos->x, die_pos->y, die_pos->x);
            int failure = assassin_acquired(map, Deidara, Saken, trap_pos, die_pos, h, w, &dir, status);
            if (failure == 1)
            {
                fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status);
            }
            status->tunnel_kill = 0;
        }
        else if (status->tp_on_map == 1 && map[tp_on_map_pos->y][tp_on_map_pos->x].path < map[tp_on_map_pos->y][tp_on_map_pos->x].enemy_path - 1 && Deidara->f_t == 0 && current_number >= 10)
        {
            find_dir(map, Deidara, Saken, tp_on_map_pos, h, w, &dir, current_number, status);
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
                find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status);
            }
            status->assassin = 0;
        }
        else if (status->assassin == 1) ////????????????????????????????????????????????????????????????????????????????
        {
            fprintf(stderr, "Final aim to assassin is %d %d %d Enemy %d %d\n", map[trap_pos->y][trap_pos->x].goal, trap_pos->y, trap_pos->x, die_pos->y, die_pos->x);
            int failure = assassin_acquired(map, Deidara, Saken, trap_pos, die_pos, h, w, &dir, status);
            if (failure == 1)
            {
                fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
                find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status);
            }
            status->tunnel_kill = 0;
        }
        else if (map[Deidara->y][Deidara->x].bomb_tick == 5 && Saken->f_t == 1)
        {
            dir = 5;
        }
        else
        {
            fprintf(stderr, "Final aim is %d %d %d\n", map[aim->y][aim->x].goal, aim->y, aim->x);
            find_dir(map, Deidara, Saken, aim, h, w, &dir, current_number, status);
            status->tunnel_kill = 0;
            status->assassin = 0;
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
        free(escape_pos1);
        free(enemy_next);
        free(enemy_aim_1);
        free(enemy_aim_2_1);
        free(enemy_aim_2_2);
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