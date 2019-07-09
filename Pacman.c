#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Miscellaneous functions (rand, malloc, srand)*/
#include <ncurses.h> /* Screen handling and optimisation functions */
#include <getopt.h> /* get options from system argc/argv */
#include <string.h> /* Strings functions definitions */
#include <assert.h> /* Verify assumptions with assert */
#include <unistd.h> /* UNIX standard function */
#include "Pacman.h" /* To be created for this template if needed */
#include <time.h> /* Time and date functions */
#include <math.h> /* Mathematics functions */

void help(void)
{
    IFDEBUG("help()");
    printf("%s - %s\n", "upecman", "Pacman Ncurses");
    printf("\nUsage: ./%s [-h|-v|-c]\n", "upecman");
    printf("\nOptions:\n");
    printf("\t-h,  --help\n\t\tShow this help.\n");
    printf("\t-c,  --copyright, --version\n\t\tShow version and copyright information.\n");
    printf("\t-v,  --verbose\n\t\tSet verbose level (cumulative).\n");
    /* add more options here */
    printf("\nExit status:\n\t0 if ok.\n\t1 some error occurred.\n");
    printf("\nTodo:\n\tLong options not implemented yet.\n");
    printf("\nAuthor:\n\tWritten by %s <%s>\n\n", "Ruben Carlo Benante", "rcb@beco.cc");
    exit(EXIT_FAILURE);
}

void copyr(void)
{
    IFDEBUG("copyr()");
    printf("%s - Version %s\n", "upecman", VERSION);
    printf("\nCopyright (C) %d %s <%s>, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>. This  is  free  software:  you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. USE IT AS IT IS. The author takes no responsability to any damage this software may inflige in your data.\n\n", 2016, "Ruben Carlo Benante", "rcb@beco.cc");
    if(verb > 3) printf("copyr(): Verbose: %d\n", verb); /* -vvvv */
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{	
	IFDEBUG("main()\n");
    int opt; /* return from getopt() */
    t_game g; /* the game */
    t_direction pacdirec = left;
    int scrx, scry; /* variaveis de posicao do labirinto na tela */
    const char *sready = "Voce esta pronto? (s/n)"; /* ready string */
    int kin = 0; /* entrada de teclado */
    clock_t start_t;
    int input;

    IFDEBUG("Starting optarg loop...\n");

    /* g
     * etopt() configured options:
     *        -h  help
     *        -c  copyright & version
     *        -v  verbose
     */
    opterr = 0;
    while((opt = getopt(argc, argv, "hvc")) != EOF)
        switch(opt)     
        { 
            case 'h':
                help();
                break;
            case 'v':
                verb++;
                break;
            case 'c':
                copyr();
                break;
            case '?':
            default:
                printf("Type\n\t$man %s\nor\n\t$%s -h\nfor help.\n\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

    if(verb)
        printf("Verbose level set at: %d\n", verb);

    /* ...and we are done */
    IFDEBUG("Starting the game now...\n");

    /*iniciando*/
    g = upecman_init();
    colorir(); //colorindo mapa
	getmaxyx(stdscr, scry, scrx); //centralizando
	
	printlab(g, scry, scrx); //printando labirinto
	g.lab[17][9] = ' ';

	//printando frases na tela
	attron(COLOR_PAIR(6));
	mvprintw(scry/2-19, scrx/2-4, "PACMAN!!");
	attroff(COLOR_PAIR(6));
	mvprintw(scry/2-16, scrx/2-11, "%s", sready);
	refresh();

	while(1) //pegando entrada de usuario e tela de início de jogo
	{
		kin = getch();
		if(kin == 's')
		{
			break;
		}
		if(kin == 'n' || kin == 'e')
		{
           endwin();
            return EXIT_SUCCESS;
        }
        usleep(16667); /* wait 16666 microseconds to avoid frying the processor (1/60 s) */
    }
    
    start_t = clock(); //iniciando relogio

    while(kin != 'e') //iniciando jogo
    {
        usleep(250000);
        /* Keyboard input for pacman directions */
        kin = getch();
        if(kin == KEY_LEFT)
            pacdirec = left;
        if(kin == KEY_RIGHT)
            pacdirec = right;
        if(kin == KEY_DOWN)
            pacdirec = down;
        if(kin == KEY_UP)
            pacdirec = up;

        g.pacman = movimentacaoPac(g,pacdirec);
        printlab(g, scry, scrx);

        if ((isAfraid == 0) || (isAfraid == 1 && clock() - afraid_t > 16000)) 
        {
            isAfraid = 0;
            g = modosTime(g, start_t, scry, scrx);
        }
        
        g = pontuacao(g);
        g = moveGhost(g);
        
    if(g.pacman.pontos == 188)
    {
        endwin();
        while(1)
        {
            attron(COLOR_PAIR(1));
            mvprintw(scry/2, scrx/2-4, "VITORIA!");
            mvprintw(scry/2+1, scrx/2-8, "Aperte E para sair");
            attroff(COLOR_PAIR(1));
            input = getch();
            if(input == 'e')
            {
                endwin();
                return EXIT_SUCCESS;
            }
        }
    }
        if(g.pacman.life <= 0)
        {
            if(fimPac(g, scry, scrx) == 'n')
            {
               endwin();
               return EXIT_SUCCESS;
            }
            else 
            {       
                g = upecman_init();
                g.lab[17][9]=' ';
            }
        }
    }
   		 endwin();
    return EXIT_SUCCESS;
}

t_game pontuacao(t_game g)
{	
	int f=0, i=0, x = g.pacman.pos.x, y = g.pacman.pos.y;

	if(g.lab[y][x] == '.')
	{
		g.lab[y][x] = ' ';
		g.pacman.pontos++;
		g.pacman.score = g.pacman.score + 10;
	}

	if(g.lab[y][x] == 'o')
	{
		isAfraid = 1;
		afraid_t = clock();
		g.lab[y][x] = ' ';
		g.pacman.pontos++;
		g.pacman.score = g.pacman.score + 50;

		for(f=blinky;f<=clyde;f++)
			g.ghost[f].mode = afraid;
	}

	for(i=blinky;i<=clyde;i++) /*interação entre pacman e fantasma*/
	{
	    if((g.ghost[i].mode != afraid) && (g.ghost[i].mode != dead))
       	{          
            if((g.pacman.pos.y == g.ghost[i].pos.y && g.pacman.pos.x == g.ghost[i].pos.x) || (g.pacman.dir == left && g.ghost[i].dir == right && g.pacman.pos.x == g.ghost[i].pos.x-1 && g.pacman.pos.y == g.ghost[i].pos.y) || (g.pacman.dir == right && g.ghost[i].dir == left && g.pacman.pos.x == g.ghost[i].pos.x+1 && g.pacman.pos.y == g.ghost[i].pos.y) || (g.pacman.dir == up && g.ghost[i].dir == down && g.pacman.pos.x == g.ghost[i].pos.x && g.pacman.pos.y == g.ghost[i].pos.y-1) || (g.pacman.dir == down && g.ghost[i].dir == up && g.pacman.pos.x == g.ghost[i].pos.x && g.pacman.pos.y == g.ghost[i].pos.y+1))             
            {
                usleep(1000000);
                g.pacman.life--;
               	g.pacman.pos.x = 9;
               	g.pacman.pos.y = 17;
               	g.pacman.dir = left;

               	g.ghost[blinky].pos.y = 7;
               	g.ghost[blinky].pos.x = 9;
               	g.ghost[pinky].pos.y = 9;
               	g.ghost[pinky].pos.x = 10;
               	g.ghost[inky].pos.y = 10;
               	g.ghost[inky].pos.x = 10;
               	g.ghost[clyde].pos.y = 11;
               	g.ghost[clyde].pos.x = 10;
           	}
       	}
       	else
           	if((g.ghost[i].mode == afraid) && (g.ghost[i].mode != dead))
           	{                	
           		if((g.pacman.pos.y == g.ghost[i].pos.y && g.pacman.pos.x == g.ghost[i].pos.x) || (g.pacman.dir == left && g.ghost[i].dir == right && g.pacman.pos.x == g.ghost[i].pos.x-1 && g.pacman.pos.y == g.ghost[i].pos.y) || (g.pacman.dir == right && g.ghost[i].dir == left && g.pacman.pos.x == g.ghost[i].pos.x+1 && g.pacman.pos.y == g.ghost[i].pos.y) || (g.pacman.dir == up && g.ghost[i].dir == down && g.pacman.pos.x == g.ghost[i].pos.x && g.pacman.pos.y == g.ghost[i].pos.y-1) || (g.pacman.dir == down && g.ghost[i].dir == up && g.pacman.pos.x == g.ghost[i].pos.x && g.pacman.pos.y == g.ghost[i].pos.y+1))             
               	{
                   	g.pacman.score = g.pacman.score + 750;
                   	g.ghost[i].mode = dead;
               	}
           	}
    }

    //g = cereja(g,start_t);
    if(g.lab[y][x] == '%')
	{
		g.lab[y][x] = ' ';
		g.pacman.score = g.pacman.score + 500;
	}

	return g;
}

/*t_game cereja(t_game g, clock_t start_t)
{
	
	clock_t now_t, total_t;
	now_t = clock();
    total_t = (now_t - start_t);

    if(total_t == 10000000)
    	mvprintw(13, 10, "%");

    return g;
}*/

t_game upecman_init(void)
{
    IFDEBUG("init()");
    /* initialization */

    t_game g;
    int f, y;

    for(y = 0; y < LABL; y++)
        strcpy(g.lab[y], labmodel[y]); /* copy the labyrinth (lines of strings) */

    g.pacman.pos.y = 17; /* posicao inicial do pacman na coordenada Y */
    g.pacman.pos.x = 9; /* posicao inicial do pacman na coordenada X */
    g.pacman.dir = left; /* direcao inicial do pacman */
    g.pacman.life = 3; /* vidas iniciais do pacman = 3 */
    g.pacman.score = 0; /* pontuacao inicial do pacman = 0 */
    g.pacman.pontos = 0; /* numero de pellets comido */

    for(f = blinky; f <= clyde; f++) /* blinky, pinky, inky and clyde */
     {
        switch(f)
         {
            case blinky:
                g.ghost[f].pos.y = 7; /* blinky start position (line) */
                g.ghost[f].pos.x = 9; /* blinky start position (column) */
                g.ghost[f].starget.y = 0; /* preferred corner (line) */
                g.ghost[f].starget.x = 19; /* preferred corner (column) */
                break;
            case pinky:
                g.ghost[f].pos.y = 9; /* pinky start position (line) */
                g.ghost[f].pos.x = 10; /* pinky start position (column) */
                g.ghost[f].starget.y = 0; /* preferred corner (line) */
                g.ghost[f].starget.x = 0; /* preferred corner (column) */
                break;
            case inky:
                g.ghost[f].pos.y = 10; /* inky start position (line) */
                g.ghost[f].pos.x = 10; /* inky start position (column) */
                g.ghost[f].starget.y = 22; /* preferred corner (line) */
                g.ghost[f].starget.x = 19; /* preferred corner (column) */
                break;
            case clyde:
                g.ghost[f].pos.y = 11; /* clyde start position (line) */
                g.ghost[f].pos.x = 10; /* clyde start position (column) */
                g.ghost[f].starget.y = 22; /* preferred corner (line) */
                g.ghost[f].starget.x = 0; /* preferred corner (column) */
     
         }
        g.ghost[f].dir = left; /* start direction and future direction: left */
        g.ghost[f].mode = scatter;
     }
    return g;
}

void printlab(t_game g, int scry, int scrx)
{
    IFDEBUG("printlab()");

    int f;

    g.lab[7][9] = ' ';
    g.lab[9][10] = ' ';
    g.lab[10][10] = ' ';
    g.lab[11][10] = ' ';

    clear(); /* clear the screen */

    colorePonto(g, scry, scrx);

    mvprintw(scry/2-2, scrx/2+12, "Modo do fantasma 0 = chase");
    mvprintw(scry/2-1, scrx/2+12, "Modo do fantasma 1 = scatter");
    mvprintw(scry/2, scrx/2+12, "Modo do fantasma 2 = afraid");

    mvprintw(scry/2+15, scrx/2-10, "Pontos Comidos: %d", g.pacman.pontos); /* pontos comidos*/
    mvprintw(scry/2+13, scrx/2-10, "Vidas: %d", g.pacman.life); /* vidas */
    mvprintw(scry/2-12, scrx/2-10, "Pontuacao: %d", g.pacman.score); /* score */
    mvprintw(g.pacman.pos.y+scry/2-11, g.pacman.pos.x+scrx/2-10, "@"); /* imprimir posicao do pacman*/
    
    attron(COLOR_PAIR(6));
    mvprintw(scry/2+20, scrx/2-9, "Aperte E para sair"); /*indicando botao de saida */
    attroff(COLOR_PAIR(6));

	attron(COLOR_PAIR(3));
    mvprintw(scry/2+17, scrx/2-13, "Use o Keypad para se mover");
    attroff(COLOR_PAIR(3));


    for(f=blinky;f<=clyde;f++) /* blinky, pinky, inky and clyde */
    {
        mvprintw(scry/2+12, scrx/2-10, "Modo dos Fantasmas: %d", g.ghost[f].mode);
        if(f == blinky) //vermelho
        {
            mvprintw(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10,"B");
            mvchgat(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10, 1, A_BOLD, 1, NULL);
        }

        if(f == pinky) //magenta
        {
            mvprintw(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10, "P");
            mvchgat(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10, 1, A_BOLD, 2, NULL);
        }

        if(f == inky) //azul claro
        {
            mvprintw(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10,"I");
            mvchgat(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10, 1, A_BOLD, 3, NULL);
        }

        if(f == clyde) //verde
        {
            mvprintw(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10, "C");
            mvchgat(g.ghost[f].pos.y + scry/2-11, g.ghost[f].pos.x+scrx/2-10, 1, A_BOLD, 4, NULL);
        }
        if(g.ghost[f].mode == afraid)
        {
            mvchgat(g.ghost[f].pos.y+scry/2-11, g.ghost[f].pos.x+scrx/2-10, 1, A_BOLD, 5, NULL); /*Cores dos fantasmas no modo afraid*/
        }
   
    }
        mvchgat(g.pacman.pos.y + scry/2-11, g.pacman.pos.x+scrx/2-10, 1, A_BOLD, 6, NULL); /* cores do pacman*/

    refresh();
}

void coloreJogo(void)
{    
    initscr(); /* start ncurses mode screen */
    cbreak(); /* stop line buffering */
    keypad(stdscr, TRUE); /* grab extended keys */
    noecho(); /* stop echoing chars */
    curs_set(0); //desabilitando cursor
    timeout(0); /* do not wait */
    start_color(); /* initialize the color functionality */

    /* Initialize all the colors */
    init_pair(1, COLOR_RED, COLOR_BLACK); /* Blinky */
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK); /* Pinky */
    init_pair(3, COLOR_CYAN, COLOR_BLACK); /* Inky */
    init_pair(4, COLOR_GREEN, COLOR_BLACK); /* Clyde doesn't have orange :( */
    init_pair(5, COLOR_BLUE, COLOR_BLACK); /* Afraid mode: blue with A_BLINK */
    init_pair(6, COLOR_YELLOW, COLOR_BLACK); /* Pacman */
    init_pair(7, COLOR_WHITE, COLOR_BLACK); /* Cherry */
}

void colorePonto(t_game g, int scry, int scrx)
{
    int y,k;

    /*pintar o labirinto de azul */
    attron(COLOR_PAIR(5));
    for(y = 0; y < LABL; y++)
        mvprintw(scry/2+y-11, scrx/2-10, "%s\n", g.lab[y]);
    attroff(COLOR_PAIR(5));

    /* pintar os pellets de amarelo */
    for(y = 0; y < LABL; y++)
        for(k = 0; k < LABC; k++)
            if(g.lab[y][k] == '.')
                mvchgat(scry/2+y-11, scrx/2-10+k, 1, A_BOLD, 0, NULL);
    for(y = 0; y < LABL; y++)
        for(k=0; k < LABC; k++)
            if(g.lab[y][k] == 'o')
                mvchgat(scry/2+y-11, scrx/2-10+k, 1, A_BOLD, 6, NULL);   
}

t_pacman movimentacaoPac(t_game g, t_direction pacdirec)
{
    int aumentaX = 0, aumentaY = 0;

    if(pacdirec == left)
        if(g.lab[g.pacman.pos.y][g.pacman.pos.x - 1] != '#')
            g.pacman.dir = left;
    if(pacdirec == right)
        if(g.lab[g.pacman.pos.y][g.pacman.pos.x + 1] != '#')
            g.pacman.dir = right;
    if(pacdirec == up)
        if(g.lab[g.pacman.pos.y - 1][g.pacman.pos.x] != '#')
            g.pacman.dir = up;
    if(pacdirec == down)
        if(g.lab[g.pacman.pos.y + 1][g.pacman.pos.x] != '#')
            g.pacman.dir = down;

    if(g.pacman.dir == left)
    {
        if(g.pacman.pos.x == 0 && g.pacman.pos.y == 10)
        { 
            g.pacman.pos.x = 19;
            g.pacman.pos.y = 10;
        }
        if(g.lab[g.pacman.pos.y][g.pacman.pos.x - 1] != '#')
        { 
            aumentaX = -1;
            aumentaY = 0;
        }
        else
        {
            aumentaX = 0;
            aumentaY = 0;
        }
    }

    if(g.pacman.dir == right)
    {
        if(g.pacman.pos.x == 19 && g.pacman.pos.y == 10)
        {
            g.pacman.pos.x = 0;
            g.pacman.pos.y = 10;
        }
        if(g.lab[g.pacman.pos.y][g.pacman.pos.x + 1] != '#')
        { 
            aumentaX = +1;
            aumentaY = 0;
        }
        else
        {
            aumentaX = 0;
            aumentaY = 0;
        }
    }

    if(g.pacman.dir == up)
    {
        if(g.lab[g.pacman.pos.y - 1][g.pacman.pos.x] != '#')
        {
            aumentaX = 0;
            aumentaY = -1;
        }
        else
        {
            aumentaX = 0;
            aumentaY = 0;
        }
    }

    if(g.pacman.dir == down)
    {
        if(g.lab[g.pacman.pos.y + 1][g.pacman.pos.x] != '#' && g.lab[g.pacman.pos.y + 1][g.pacman.pos.x] != '-')
        {
            aumentaX = 0;
            aumentaY = +1;
        }
        else
        {
            aumentaX = 0;
            aumentaY = 0;
        }
    }

    g.pacman.pos.y += aumentaY;
    g.pacman.pos.x += aumentaX;

    return g.pacman; 
}

t_game tempoModos(t_game g, clock_t start_t, int scry, int scrx)
{
    clock_t now_t, total_t;
    for(int i = blinky; i <= clyde; i++)
    {
        now_t = clock();
        total_t = (now_t - start_t);
               
        if(total_t > 0 && total_t <= 21000)
            g.ghost[i].mode = scatter;
        if(total_t > 21000 && total_t <= 81000)
            g.ghost[i].mode = chase;
        if(total_t > 81000 && total_t <= 102000)
            g.ghost[i].mode = scatter;
        if(total_t > 102000 && total_t <= 162000)
            g.ghost[i].mode = chase;
        if(total_t > 162000 && total_t <= 177000)
            g.ghost[i].mode = scatter;
        if(total_t > 177000 && total_t <= 237000)
            g.ghost[i].mode = chase;
        if(total_t > 237000 && total_t <= 252000)
            g.ghost[i].mode = scatter;
        if(total_t > 252000)
            g.ghost[i].mode = chase;
    }

    if(total_t >= 1000000)
    	mvprintw(scry/2-18, scrx/2-1, "%");

    return g; 
}

t_game moveGhost(t_game g)
{
    int i, dy, dx;

    for(i = blinky; i <= clyde; i++)
    {
        switch(i)
        {
            case blinky:
                if(g.ghost[i].mode == dead)
                {
                    g.ghost[i].pos.y = 10;
                    g.ghost[i].pos.x = 10;
                    g.ghost[i].mode = chase;
                    continue;
                }
                if(g.ghost[i].mode == scatter)
                {
                    g.ghost[i].starget.y = 0;
                    g.ghost[i].starget.x = 19;
                }
                if(g.ghost[i].mode == afraid)
                {
                    g.ghost[i].starget.y = rand()%20;
                    g.ghost[i].starget.x = rand()%20;
                }
                if(g.ghost[i].mode == chase)
                {
                    g.ghost[i].starget.y = g.pacman.pos.y;
                    g.ghost[i].starget.x = g.pacman.pos.x;
                }
                break;
             case pinky:
                if(g.ghost[i].pos.y > 7 && g.ghost[i].pos.y <= 11 && g.ghost[i].pos.x == 10)
                {
                    g.ghost[i].pos.y--;
                    continue;
                }
                if(g.ghost[i].mode == dead)
                {
                    g.ghost[i].pos.y = 9;
                    g.ghost[i].pos.x = 10;
                    g.ghost[i].mode = chase;
                    continue;
                }
                if(g.ghost[i].mode == scatter)
                {
                    g.ghost[i].starget.y = 0;
                    g.ghost[i].starget.x = 0;
                }
                if(g.ghost[i].mode == afraid)
                {
                    g.ghost[i].starget.y = rand()%20;
                    g.ghost[i].starget.x = rand()%20;
                }
                if(g.ghost[i].mode == chase) /*Necessaria posicao do pacman*/
                {
                    if(g.pacman.dir == up)
                    {
                        g.ghost[i].starget.x = g.pacman.pos.x-4;

                        if(g.pacman.pos.y+4 > 22)
                            g.ghost[i].starget.y = 22;
                        else
                            g.ghost[i].starget.y = g.pacman.pos.y+4;
                    }
                    if(g.pacman.dir == down)
                    {
                        g.ghost[i].starget.x = g.pacman.pos.x;

                        if(g.pacman.pos.y-4 < 0)
                            g.ghost[i].starget.y = 0;
                        else
                            g.ghost[i].starget.y = g.pacman.pos.y-4;
                    }
                    if(g.pacman.dir == left)
                    {
                        g.ghost[i].starget.y = g.pacman.pos.y;

                        if(g.pacman.pos.x-4 < 0)
                            g.ghost[i].starget.x = 0;
                        else
                            g.ghost[i].starget.x = g.pacman.pos.x-4;
                    }
                    if(g.pacman.dir == right)
                    {
                        g.ghost[i].starget.y = g.pacman.pos.y;

                        if(g.pacman.pos.x+4 > 19)
                            g.ghost[i].starget.x = 19;
                        else
                            g.ghost[i].starget.x = g.pacman.pos.x+4;
                    }
                }
                break;
             case clyde:
                if(g.ghost[i].pos.y > 7 && g.ghost[i].pos.y <= 11 && g.ghost[i].pos.x ==10)
                {
                    g.ghost[i].pos.y--;
                    continue;
                }
                if(g.ghost[i].mode == dead)
                {
                    g.ghost[i].pos.y = 11;
                    g.ghost[i].pos.x = 10;
                    g.ghost[i].mode = chase;
                    g.ghost[i].dir = left;
                    continue;
                }
                if(g.ghost[i].mode == scatter)
                {
                    g.ghost[i].starget.y = 22;
                    g.ghost[i].starget.x = 0;
                }
                if(g.ghost[i].mode == afraid)
                {
                    g.ghost[i].starget.y = rand()%20;
                    g.ghost[i].starget.x = rand()%20;
                }
                if(g.ghost[i].mode == chase)
                {
                    dy = abs(g.pacman.pos.y - g.ghost[i].pos.y);
                    dx = abs(g.pacman.pos.x - g.ghost[i].pos.x);

                    if((dy + dx) >= 8)
                        g.ghost[i].starget = g.pacman.pos;

                    else
                    {
                        g.ghost[i].starget.y = 22;
                        g.ghost[i].starget.x = 0;
                    }
                }  
                break;
            case inky:
                if(g.ghost[i].pos.y > 7 && g.ghost[i].pos.y <= 11 && g.ghost[i].pos.x == 10)
                {
                    g.ghost[i].pos.y--;
                    continue;
                }
                if(g.ghost[i].mode == dead)
                {
                    g.ghost[i].pos.y = 10;
                    g.ghost[i].pos.x = 10;
                    g.ghost[i].mode = chase;
                    continue;
                }
                if(g.ghost[i].mode == scatter)
                {
                    g.ghost[i].starget.y = 22;
                    g.ghost[i].starget.x = 19;
                }
                if(g.ghost[i].mode == afraid)
                {
                    g.ghost[i].starget.y = rand()%20;
                    g.ghost[i].starget.x = rand()%20;
                }
                if(g.ghost[i].mode == chase)
                {
                    dy = g.pacman.pos.y - g.ghost[blinky].pos.y;
                    dx = g.pacman.pos.x - g.ghost[blinky].pos.x;

                    if(g.pacman.dir == up)
                    {
                        g.ghost[i].starget.x = g.pacman.pos.x + dx;

                        if(g.pacman.pos.y+2 + 2*dy > 22)
                            g.ghost[i].starget.y = 22;
                        else
                            g.ghost[i].starget.y = g.pacman.pos.y+2 + 2*dy;
                    }
                    if(g.pacman.dir == down)
                    {
                        g.ghost[i].starget.x = g.pacman.pos.x + dx;

                        if(g.pacman.pos.y-2 - 2*dy < 0)
                            g.ghost[i].starget.y = 0;
                        else
                            g.ghost[i].starget.y = g.pacman.pos.y-2 - 2*dy;
                    }

                    if(g.pacman.dir == left)
                    {
                        g.ghost[i].starget.y = g.pacman.pos.y + dy;

                        if(g.pacman.pos.x-2 - 2*dx < 0)
                            g.ghost[i].starget.x = 0;
                        else
                            g.ghost[i].starget.x = g.pacman.pos.x-2 - 2*dx;
                    }
                    if(g.pacman.dir == right)
                    {
                        g.ghost[i].starget.y = g.pacman.pos.y + dy;

                        if(g.pacman.pos.x+2 + 2*dx >20)
                            g.ghost[i].starget.x = 20;
                        else
                            g.ghost[i].starget.x = g.pacman.pos.x+2 + 2*dx;
                    }                   
                }                
                break;
        }
        g = targetcalc(g, i);
    }
    return g;
}

t_game calcAlvo(t_game g, int i) /* Calculo da melhor rota possivel para os fantasmas */
{
    float d1 = 0, d2 = 0, d3 = 0, d4 = 0; /* Modulo das hipotenusas */
    int A = 0, B = 0, C = 0, D = 0; /* Variaveis para as 4 possibilidades de movimentacao */
    char pos = ' ';
    
    if((g.lab[g.ghost[i].pos.y][g.ghost[i].pos.x+1] != '#' && '-') && (g.ghost[i].dir != left))
    {
        A = 1;
        d1 = sqrt(pow(((g.ghost[i].pos.y) - g.ghost[i].starget.y),2) + pow((g.ghost[i].pos.x+1 - g.ghost[i].starget.x),2)); /*Calculo da hipotenusa*/
    }
    if((g.lab[g.ghost[i].pos.y][g.ghost[i].pos.x-1] != '#' && '-') && (g.ghost[i].dir != right))
    {
        B = 1;
        d2 = sqrt(pow((g.ghost[i].pos.y - g.ghost[i].starget.y),2) + pow((g.ghost[i].pos.x-1 - g.ghost[i].starget.x) ,2));
    }
    if((g.lab[g.ghost[i].pos.y+1][g.ghost[i].pos.x] != '#' && (g.lab[g.ghost[i].pos.y+1][g.ghost[i].pos.x] != '-') && (g.ghost[i].dir != up)))
    {
        C = 1;
        d3 = sqrt(pow((g.ghost[i].pos.y+1 - g.ghost[i].starget.y),2) + pow((g.ghost[i].pos.x  - g.ghost[i].starget.x) ,2));
    }
    if((g.lab[g.ghost[i].pos.y-1][g.ghost[i].pos.x] != '#' && '-' ) && (g.ghost[i].dir != down))
    {
        D = 1;
        d4 = sqrt(pow((g.ghost[i].pos.y-1 - g.ghost[i].starget.y),2) + pow((g.ghost[i].pos.x  - g.ghost[i].starget.x) ,2));
    }

    /* Teleporte de fantasma */
    if(g.ghost[i].pos.x == 0 && g.ghost[i].pos.y == 10)
    {
        g.ghost[i].pos.x = 18, g.ghost[i].pos.y = 10;
    }
    if(g.ghost[i].pos.x == 19 && g.ghost[i].pos.y == 10)
    {
        g.ghost[i].pos.x = 1, g.ghost[i].pos.y = 10;
    }

    if(A==1 && B==1) /* caso exista mais de uma possibilidade de caminho */
        pos = 'E';
    if(A==1 && C==1)
        pos = 'F';
    if(A==1 && D==1)
        pos = 'G';
    if(B==1 && C==1)
        pos = 'H';
    if(B==1 && D==1)
        pos = 'I';
    if(C==1 && D==1)
        pos = 'J';
    if(A==1 && B==1 && C==1)
        pos = 'K';
    if(A==1 && B==1 && D==1)
        pos = 'L';
    if(C==1 && B==1 && D==1)
        pos = 'M';
    if(A==1 && C==1 && D==1)
        pos = 'N';
    if(A==1 && B==1 && D==1 && C==1)
        pos = 'O';

    if(A==1 && B==0 && C==0 && D==0)
    {
        g.ghost[i].pos.x++;
        g.ghost[i].dir = right;
    }
    if(A==0 && B==1 && C==0 && D==0)
    {
        g.ghost[i].pos.x--;
        g.ghost[i].dir = left;
    }
    if(A==0 && B==0 && C==1 && D==0)
    {
        g.ghost[i].pos.y++;
        g.ghost[i].dir = down;
    }
    if(A==0 && B==0 && C==0 && D==1)
    {
        g.ghost[i].pos.y--;
        g.ghost[i].dir = up;
    }

    switch(pos)
    {
        case('E'):
        {
            if(d1 < d2)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            else
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            break;
        }
        case('F'):
        {
            if(d1 < d3)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            else
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            }
            break;
        }
        case('G'):
        {
            if(d1 < d4)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            else
            {
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;
            }
            break;
        }
        case('H'):
        {
            if(d2 < d3)
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            else
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            }
            break;
        }
        case('I'):
        {
            if(d2 < d3)
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            else
            {
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;

            }
            break;
        }
        case('J'):
        {
            if(d3 < d4)
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            } 
            else
            { 
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;
            }
            break;
        }
        case('K'):
        {
            if(d1 < d2 && d1 <= d3)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            if(d2 < d3 && d2 <= d1)
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            else
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            }
            break;
        }
        case('L'):
        {
            if(d1 < d2 && d1 < d4)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            if(d2 < d4 && d2 < d1)
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            else
            {
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;
            }
            break;
        }
        case('M'):
        {
            if(d3 < d2 && d3 < d4)
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            }
            if(d2 < d3 && d2 < d4)
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            else
            {
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;
            }
            break;
        }
        case('N'):
        {
            if(d1 < d3 && d1 < d4)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            if(d3 < d1 && d3 < d4)
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            }
            else
            {
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;
            }
            break;
        }
        case('O'):
        {
            if(d1 < d3 && d1 < d4 && d1 < d2)
            {
                g.ghost[i].pos.x++;
                g.ghost[i].dir = right;
            }
            if(d2 < d3 && d2 < d4 && d2 < d1)
            {
                g.ghost[i].pos.x--;
                g.ghost[i].dir = left;
            }
            if(d3 < d1 && d3 < d4 && d3 < d2)
            {
                g.ghost[i].pos.y++;
                g.ghost[i].dir = down;
            }
            else
            {
                g.ghost[i].pos.y--;
                g.ghost[i].dir = up;
            }
            break;
        }
        
        default:
            break;

    }

    return g;
}

char fimJogo(t_game g, int scry, int scrx)
{
    int input;
    if(g.pacman.life <= 0)
    {
        endwin();
        while(1)
        {
            attron(COLOR_PAIR(1));
            mvprintw(scry/2, scrx/2-4, "VOCE PERDEU!");
            mvprintw(scry/2+1, scrx/2-11, "Quer tentar novamente? (s/n)");
            attroff(COLOR_PAIR(1));
            input = getch();
            if(input == 's')
            return 's';
            else if(input == 'n')
            return 'n';

            initscr();
        }
    }
    else
        return 'c';
}

