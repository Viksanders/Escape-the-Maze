#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//saving point coordinates in this structure
typedef struct s_Coords
{
    int x;
    int y;
} Coords;


//stack as a point coordinates storage
struct Stack
{
    unsigned size;
    unsigned pointer;
    Coords* arr;
};

void init_stack(struct Stack* stack, int sz)
{
    stack->size = sz;
    stack->pointer = 0; //empty
    stack->arr = (Coords*)malloc(sz*sizeof(Coords));
}

void free_stack(struct Stack* stack)
{
    stack->size = 0;
    free(stack->arr);
}

void push_back(struct Stack* stack, Coords coord)
{
    if (stack->pointer >= stack->size - 1) return;  //full
    stack->pointer++;
    stack->arr[stack->pointer] = coord;
}

Coords pop_top(struct Stack* stack)
{
    Coords coord = stack->arr[stack->pointer];
    stack->pointer--;
    return coord;
}

int is_full_or_empty(struct Stack* stack)
{
    if (stack->pointer >= stack->size - 1) return 1;    //full
    if (stack->pointer == 0) return -1;     //empty
    return 0;   //ok
}
//end of stack code

//debug fcn
void print2d(int** arr, int y, int x)
{
    for(int i = 0; i < y; i++)
    {
        for(int j = 0; j < x; j++)
        {
            printf("%d\t", arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void free_2d_arr(int** arr, int height)
{
    for(int i = 0; i < height; i++)
    {
        free(arr[i]);
    }
    free(arr);
}

//this structure is given in kata
typedef struct s_Maze
{
    unsigned height, width;
    char** grid;
} Maze;

void free_maze(Maze* maze)
{
    for(unsigned i = 0; i < maze->height; i++)
    {
        free(maze->grid[i]);
    }
    free(maze->grid);
}

//debug
void print_maze(const Maze* maze)
{
    printf("\nheight = %u, width = %u\n", maze->height, maze->width);
    for(unsigned i = 0; i < maze->height; i++)
    {
        for(unsigned j = 0; j < maze->width; j++)
        {
            printf("%c", maze->grid[i][j]);
        }
        printf("\n");
    }
}

//my not optimal fuction to load mazes from files
//this fcn is not required for kata solution
void load_maze(Maze* maze)
{
    FILE* fptr;
    fptr = fopen("maze.txt", "r");
    if (fptr == NULL)
    {
        puts("Error, cant open");
        return;
    }

    int ch;
    unsigned counter_1 = 0;
    unsigned counter_2 = 0;

    while( (ch = fgetc(fptr)) != '\n' && !feof(fptr) )
    {
        counter_1++;    //+1 col
    }

    if (counter_1 == 0)
    {
        maze->width = 0;
        maze->height = 0;
        maze->grid = NULL;
        puts("file is empty");
        fclose(fptr);
        return;
    }

    maze->width = counter_1;
    counter_2++;    //+1 row

    while(!feof(fptr))
    {
        if ((ch = fgetc(fptr)) == '\n') counter_2++;    //+1 row
    }
    maze->height = counter_2;

    //printf("There are %u rows and %u cols\n", maze->height, maze->width);
    maze->grid = (char**)malloc(maze->height*sizeof(char*));
    for(unsigned i = 0; i < maze->height; i++)
    {
        maze->grid[i] = (char*)malloc(maze->width*sizeof(char));
    }

    fseek(fptr, 0, SEEK_SET);

    for(int i = 0; i < (int)maze->height; i++)
    {
        for(int j = 0; j < (int)maze->width; j++)
        {
            ch = fgetc(fptr);
            if (ch == '\n')
            {
                j--;
                continue;
            }
            //printf("%c", (char)ch);
            if (ch == '_') ch = ' ';
            maze->grid[i][j] = (char)ch;
        }
        //printf("\n");
    }

    fclose(fptr);
}

//finding start point and direction: < > v or ^
void find_start(const Maze* maze, int* x, int* y, char* ch)
{
    for(int i = 0; i < (int)maze->height; i++)
    {
        for(int j = 0; j < (int)maze->width; j++)
        {
            if (maze->grid[i][j] == '^')
            {
                *x = j;
                *y = i;
                *ch = '^';
                return;
            }
            if (maze->grid[i][j] == '>')
            {
                *x = j;
                *y = i;
                *ch = '>';
                return;
            }
            if (maze->grid[i][j] == '<')
            {
                *x = j;
                *y = i;
                *ch = '<';
                return;
            }
            if (maze->grid[i][j] == 'v')
            {
                *x = j;
                *y = i;
                *ch = 'v';
                return;
            }
        }
    }
}

//at the beginning we have stack with points to check (check_now), and an empty stack (check_next)
//the algorithm is takes a point from stack, scans it across and adds new points to be checked
//if we are at the border, then the solution has been found
void make_step(int** arr, int height, int width, int step_val, const Maze* maze, int* flag, struct Stack* check_now, struct Stack* check_next, int* x_end, int* y_end)
{
    Coords NOW, NEXT;
    while(1)
    {
        if (is_full_or_empty(check_now) == -1) break;
        NOW = pop_top(check_now);
        //printf("now.x = %d, now.y = %d\n", NOW.x, NOW.y);
        for(int i = NOW.y - 1; i <= NOW.y + 1; i++)
        {
            if ((i < 0) || (i >= height)) continue;
            for(int j = NOW.x - 1; j <= NOW.x + 1; j++)
            {
                if ((j < 0) || (j >= width)) continue;
                if ((j == NOW.x) && (i == NOW.y)) continue;     //skipping the cell which is going to be surronded
                //skipping diagonal cells
                if (((j == NOW.x - 1) && (i == NOW.y - 1)) || ((j == NOW.x + 1) && (i == NOW.y + 1)) || ((j == NOW.x + 1) && (i == NOW.y - 1)) || ((j == NOW.x - 1) && (i == NOW.y + 1))) continue;
                if (maze->grid[i][j] == '#') continue;  //skipping the walls
                if (arr[i][j] != 0) continue;           //skipping already filled cells
                arr[i][j] = step_val + 1;               //if everything is ok, then mark step with the current step value + 1
                NEXT.x = j;
                NEXT.y = i;
                push_back(check_next, NEXT);            //and push marked point to the stack, this point will be checked in the next iteration
                if ((i == height - 1) || (j == width - 1) || (i == 0) || (j == 0))
                {
                    *flag = 1;      //end of path signalisation
                    *x_end = j;
                    *y_end = i;
                    printf("\nEXIT! X = %d, Y = %d, path length = %d\n\n", j, i, step_val + 1); //debug feature
                    return;
                }
            }
        }
    }
}

/*  //NOT-OPTIMAL SOLUTION!!!
void make_step_old(int** arr, int height, int width, int step_val, const Maze* maze, int* flag, int* x_end, int* y_end, int* x_scan_start, int* x_scan_end, int* y_scan_start, int* y_scan_end)
{
    register int i;
    register int j;
    for(i = *y_scan_start; i < *y_scan_end; i++) //skipping border
    {
        for(j = *x_scan_start; j < *x_scan_end; j++)
        {
            //passing through 2d-array maze
            if(arr[i][j] == step_val)   //surrounding each cell marked with the number of current step
            {
                //scanning local 2d array around the cell
                for(int y = i-1; y <= i+1; y++)
                {
                    if ((y < 0) || (y >= height)) continue;

                    if ((y == i - 1) && (*y_scan_start > 0) && (y <= *y_scan_start)) (*y_scan_start)--;
                    else if ((y == i + 1) && (*y_scan_end < height - 1) && (y >= *y_scan_end)) (*y_scan_end)++;

                    for(int x = j-1; x <= j+1; x++)
                    {
                        if ((x < 0) || (x >= width)) continue;
                        if ((x == j) && (y == i)) continue; //skipping the cell which is going to be surronded
                        if (((x == j-1) && (y == i-1)) || ((x == j+1) && (y == i+1)) || ((x == j-1) && (y == i+1)) || ((x == j+1) && (y == i-1))) continue; //skipping diagonal cells
                        if (maze->grid[y][x] == '#') continue;  //skipping the walls
                        if (arr[y][x] != 0) continue;       //skipping already filled cells
                        arr[y][x] = step_val + 1;           //marking cell around the target cell

                        if ((x == j - 1) && (*x_scan_start > 0) && (x <= *x_scan_start)) (*x_scan_start)--;
                        else if ((x == j + 1) && (*x_scan_end < width - 1) && (x >= *x_scan_end)) (*x_scan_end)++;

                        if ((y == height-1) || (x == width-1) || (y == 0) || (x == 0))   //exit has been found
                        {
                            *flag = 1;
                            *x_end = x;
                            *y_end = y;
                            //printf("\nEXIT! X = %d, Y = %d, path length = %d\n\n", x, y, step_val + 1);
                            return;
                        }
                    }
                }
            }
        }
    }
}
*/

void exch(char* a, char* b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void str_reverse(char* str)
{
    unsigned length = strlen(str);
    for(unsigned i = 0; i < length/2; i++)
    {
        exch(&str[i], &str[length - 1 - i]);
    }
}

//this function creates path from the beginning to the and accordion to kata conditions
//printf and puts for debug only
//the function is not optimal and have some dirty code, but im to lazy to do some refactoring
//!!! This function is neccessary for kata completion
char* back_to_beginning(int** arr, int height, int width, int x_end, int y_end, int initial_dir, int str_size, Maze* maze)
{
    char* path = (char*)calloc(str_size, sizeof(char));
    int x_coord = x_end;
    int y_coord = y_end;
    int val;
    char dir;

    if (x_end == 0)
    {
        dir = '<';
    }
    else if (x_end == width - 1)
    {
        dir = '>';
    }
    else if (y_end == 0)
    {
        dir = '^';
    }
    else if (y_end == height - 1)
    {
        dir = 'v';
    }
    else
    {
        //puts("error");
        dir = ' ';
    }

    //printf("x_end = %d, y_end = %d, init_dir = %c\n", x_end, y_end, dir);
    //*
    while(1)
    {
        //printf("x = %d, y = %d, dir = %c\n", x_coord, y_coord, dir);
        val = arr[y_coord][x_coord];
        if (val == 1)
        {
            if(dir == initial_dir)
            {
                //do nothing
            }
            else
            {
                if (initial_dir == '^')
                {
                    if (dir == '<') strcat(path, "L");
                    else if (dir == '>') strcat(path, "R");
                    else if (dir == 'v') strcat(path, "B");
                }
                else if (initial_dir == '>')
                {
                    if (dir == '<') strcat(path, "B");
                    else if (dir == '^') strcat(path, "L");
                    else if (dir == 'v') strcat(path, "R");
                }
                else if (initial_dir == '<')
                {
                    if (dir == '^') strcat(path, "R");
                    else if (dir == '>') strcat(path, "B");
                    else if (dir == 'v') strcat(path, "L");
                }
                else if (initial_dir == 'v')
                {
                    if (dir == '<') strcat(path, "R");
                    else if (dir == '>') strcat(path, "L");
                    else if (dir == '^') strcat(path, "B");
                }
            }
            break;
        }
        maze->grid[y_coord][x_coord] = '*';
        //scanning cells around current position, ALL STEPS ARE IVERTED, WE ARE MOVING FORM THE END TO THE BEGINNING
        if ((x_coord + 1 < width) && (val - arr[y_coord][x_coord + 1] == 1))   //step left
        {
            x_coord++;
            switch(dir)
            {
                case '^':
                    strcat(path, "RF");
                    break;

                case 'v':
                    strcat(path, "LF");
                    break;

                case '>':
                    strcat(path, "BF");
                    break;

                case '<':
                    strcat(path, "F");
                    break;

                default:
                    break;
            }
            dir = '<';
            continue;
        }

        if ((x_coord - 1 > 0) && (val - arr[y_coord][x_coord - 1] == 1))    //step right
        {
            x_coord--;
            switch(dir)
            {
                case '^':
                    strcat(path, "LF");
                    break;

                case 'v':
                    strcat(path, "RF");
                    break;

                case '>':
                    strcat(path, "F");
                    break;

                case '<':
                    strcat(path, "BF");
                    break;

                default:
                    break;
            }
            dir = '>';
            continue;
        }

        if ((y_coord + 1 < height) && (val - arr[y_coord + 1][x_coord] == 1))   //step forward
        {
            y_coord++;
            switch(dir)
            {
                case '^':
                    strcat(path, "F");
                    break;

                case 'v':
                    strcat(path, "BF");
                    break;

                case '>':
                    strcat(path, "RF");
                    break;

                case '<':
                    strcat(path, "LF");
                    break;

                default:
                    break;
            }
            dir = '^';
            continue;
        }

        if ((y_coord - 1 > 0) && (val - arr[y_coord - 1][x_coord] == 1))   //step back
        {
            y_coord--;
            switch(dir)
            {
                case '^':
                    strcat(path, "BF");
                    break;

                case 'v':
                    strcat(path, "F");
                    break;

                case '>':
                    strcat(path, "LF");
                    break;

                case '<':
                    strcat(path, "RF");
                    break;

                default:
                    break;
            }
            dir = 'v';
            continue;
        }
    }
    //*/
    return path;
}

char* find_path(Maze* maze)
{
    char* path = NULL;
    //creating axillary 2d array of integers for make_step() fcn
    int ax_height = (int)maze->height;
    int ax_width = (int)maze->width;
    int** ax_grid = (int**)malloc(ax_height*sizeof(int*));
    for(int i = 0; i < ax_height; i++)
    {
        ax_grid[i] = (int*)calloc(ax_width, sizeof(int));
    }

    int x_start, y_start, x_end, y_end;
    char start_direction;
    find_start(maze, &x_start, &y_start, &start_direction);
    //printf("start dir = %c\n", start_direction);
    if ((x_start == (int)maze->width - 1) || (x_start == 0) || (y_start == (int)maze->height - 1) || (y_start == 0))
    {
        //printf("Already at the exit!\n");
        return NULL;
    }

    ax_grid[y_start][x_start] = 1;  //setting start point
    int flag = 0;
    int steps = 1;
    /*  //not-optimal, skip this code
    int x_scan_start = (x_start - 1 >= 0) ? x_start - 1 : x_start;
    int x_scan_end = (x_start + 1 <= ax_width - 1) ? x_start + 1 : x_start;
    int y_scan_start = (y_start - 1 >= 0) ? y_start - 1 : y_start;
    int y_scan_end = (y_start + 1 <= ax_height - 1) ? y_start + 1 : y_start;
    */

    //creating stacks for points to be checked at this step and on the next step
    struct Stack* check_now = (struct Stack*)malloc(sizeof(check_now));
    struct Stack* check_next = (struct Stack*)malloc(sizeof(check_next));
    struct Stack* dummy_ptr = NULL;
    init_stack(check_now, 100);
    init_stack(check_next, 100);
    Coords start_point; start_point.x = x_start; start_point.y = y_start;
    push_back(check_now, start_point);

    while(flag == 0)
    {
        /*  //not optimal, skip
        if (steps > ax_height*ax_width) //maze is unsolvable
        {
            //puts("No solution");
            return NULL;
        }
        make_step_old(ax_grid, ax_height, ax_width, steps, maze, &flag, &x_end, &y_end, &x_scan_start, &x_scan_end, &y_scan_start, &y_scan_end);
        */
        if (is_full_or_empty(check_now) == -1)
        {
            free_2d_arr(ax_grid, ax_height);
            free_stack(check_now);
            free_stack(check_next);
            free(check_now);
            free(check_next);
            puts("No solution");
            return NULL;
        }
        make_step(ax_grid, ax_height, ax_width, steps, maze, &flag, check_now, check_next, &x_end, &y_end);
        steps++;
        //SWAP TWO STACKS
        dummy_ptr = check_now;
        check_now = check_next;
        check_next = dummy_ptr;
    }

    //print2d(ax_grid, ax_height, ax_width);
    path = back_to_beginning(ax_grid, ax_height, ax_width, x_end, y_end, start_direction, steps*2, maze);
    str_reverse(path);
    //printf("END: x = %d, y = %d\n", x_end, y_end);
    free_2d_arr(ax_grid, ax_height);
    free_stack(check_now);
    free_stack(check_next);
    free(check_now);
    free(check_next);

    return path;
}



int main()
{
    Maze maze;
    load_maze(&maze);
    print_maze(&maze);

    char* path = find_path(&maze);
    printf("\nPATH: %s\n", path);

    print_maze(&maze);

    free_maze(&maze);
    free(path);

    getchar();
    return 0;
}


