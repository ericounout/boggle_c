#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

int is_integer();                                                                                                  // vérifie si un nombre entré en paramètre est entier
char *copy_string();                                                                                               // utilisé pour copier un string depuis un pointeur vers un autre pointeur (utilisé pour mettre la grille entrée en argument dans une autre variable créée)
void display_grid();                                                                                               // utilisé pour afficher la grille sans les barres de "+" au dessus et en dessous
void display_grid_border();                                                                                        // utilisé pour afficher les barres de "+" au dessus et en dessous
int word_is_in_grid();                                                                                             // vérifie si le mot est dans la grille
int is_letter_around_x_position(char *grid, int square_matrix_grid_size, int x, char letter, int *word_positions); // vérifie si une lettre spécifié est autour d'une lettre entrée en paramètre dans la grille et retourne sa position (0 si elle n'est pas trouvée)
int is_position_in_word_positions();                                                                               // vérifie si une même lettre n'est pas comptabilisé 2 fois dans la grille (exemple : grille 2x2 "abcd" ou le mot serait aba)
void modify_grid();                                                                                                // Rends les lettres du mot trouvés en majuscule

int main(int argc, char const *argv[])
{
    argc = argc - 1;

    // vérification du nombre d'arguments rentrés
    if ((argc != 2 && argc != 4 && argc != 6) || (argv[1][0] != '-' || argv[1][1] != 'g'))
    {
        fprintf(stderr, "Usage: %s -g GRID [-s SIZE] [-w WORD]\n", argv[0]);
        return 84;
    }

    char *grid = copy_string(argv[2]);
    const int str_length = strlen(grid); // longueur du string entré

    int square_matrix_grid_size = 4;                        // taille d'un côté de la grille
    char *word = malloc(sizeof(char) * str_length);         // alloue l'espace maximum que le mot cherché pourrait avoir
    int *word_positions = malloc(sizeof(int) * str_length); // cette variable est un tableau contenant la position de chaque lettre du mot recherché au fur et à mesure que le programme les détecte donc a le meme espace
                                                            // alloué que le mot lui même
    int is_w_arg_enabled = 0;

    // cas de figure ou -g [GRID] n'est pas le seul argument rentré
    if (argc > 2)
    {
        for (int i = 3; i < argc; i += 2)
        {
            if (argv[i][0] == '-')
            {
                if (argv[i][1] == 's') // cas de figure ou -s [SIZE] est un des arguments
                {
                    square_matrix_grid_size = atoi(argv[i + 1]);
                }
                else if (argv[i][1] == 'w') // cas de figure ou -w [WORD] est un des arguments
                {
                    word = copy_string(argv[i + 1]);
                    is_w_arg_enabled = 1; // variable qui sera utile pour pouvoir avoir -s et -w ensemble sans rajouter trop de code superflu
                }
                else
                {
                    fprintf(stderr, "The option \"%s\" does not exist\n", argv[i]);
                    return 84;
                }
            }
            else
            {
                fprintf(stderr, "Usage: %s -g GRID -s [SIZE] -w [WORD]\n", argv[0]); // montre avec quelle syntaxe il faut utiliser les arguments
                return 84;
            }
        }
    }

    // vérifie si le nombre de lettres entrées correspond à une grille valide donc carrée à l'aide de sa racine (si sa racine est un entier elle est valide)
    if (!is_integer(sqrt(str_length)))
    {
        fprintf(stderr, "Invalid grid string length\n");
        return 84;
    }

    // vérifie si la taille entrée est la bonne (notez que si l'on veut une grille autre que 4x4 on est obligés de spécifier la taille à l'aide d'un -s [SIZE] car autrement je ne voyais pas l'utilité de cet argument)
    if (square_matrix_grid_size != sqrt(str_length))
    {
        fprintf(stderr, "Invalid grid size\n");
        return 84;
    }

    // boucle principale du jeu
    do
    {
        // vérifie si l'argument -w [WORD] est entré ou non et agit en conséquence en demandant d'entrer un mot si ce n'est pas le cas
        if (!is_w_arg_enabled)
        {
            display_grid(grid, square_matrix_grid_size);
            printf("Enter a word > ");
            scanf("%s", word);
        }

        // vérifie que le mot ne soit pas null même si je pense que ce soit impossible
        if (word == NULL)
        {
            fprintf(stderr, "Invalid word\n");

            if (!is_w_arg_enabled)
            {
                continue;
            }
            else
            {
                return 84;
            }
        }

        // vérifie que le mot ait une taille valide (pas supérieure à la taille de la grille)
        if (strlen(word) > str_length)
        {
            fprintf(stderr, "The word can have at most %d characters\n", str_length);
            if (!is_w_arg_enabled)
            {
                continue;
            }
            else
            {
                return 84;
            }
        }

        // vérifie si le mot est dans la grille ou pas
        if (word_is_in_grid(grid, square_matrix_grid_size, word, word_positions))
        {
            modify_grid(grid, strlen(word), word_positions);
            display_grid(grid, square_matrix_grid_size);
            printf("Found it\n");
            return 0;
        }
        else
        {
            printf("The word \"%s\" is not in the grid\n", word);
            if (is_w_arg_enabled) // si -w [WORD] a été entré comme argument le programme s'arrête la même si il n'a pas trouvé le mot comme dans la consigne envoyée
            {
                return 0;
            }
        }
    } while (1);

    return 0;
}

int is_integer(double val)
{
    int truncated = (int)val;
    return (val == truncated);
}

char *copy_string(const char *src)
{
    if (src == NULL)
    {
        return NULL;
    }
    char *dest = malloc(sizeof(char) * (strlen(src) + 1));

    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';

    return dest;
}

void display_grid(char *grid, int square_matrix_grid_size)
{
    display_grid_border(square_matrix_grid_size);

    for (int i = 0; i < square_matrix_grid_size; i++)
    {
        printf("| ");
        for (int j = 0; j < square_matrix_grid_size; j++)
        {
            printf("%c ", grid[i * square_matrix_grid_size + j]);
        }
        printf("|\n");
    }

    display_grid_border(square_matrix_grid_size);
}

void display_grid_border(int square_matrix_grid_size)
{
    printf("+");
    for (int i = 0; i < square_matrix_grid_size; i++)
    {
        printf("++");
    }
    printf("++\n");
}

int word_is_in_grid(char *grid, int square_matrix_grid_size, char *word, int *word_positions)
{

    for (int i = 0; i < strlen(grid); i++)
    {
        if (grid[i] == word[0]) // trouve la première lettre du mot cherché
        {
            memset(word_positions, -1, sizeof(int) * strlen(word)); // remplis le tableau de positions word_positions de -1 pour ne pas prendre en compte les itérations précédentes
            word_positions[0] = i;                                  // mets la position de la première lettre trouvée dans le tableau de positions

            if (strlen(word) == 1) // si le mot recherché ne fait qu'une lettre on s'arrête là
            {
                return 1;
            }

            int k = i; // variable qui va prendre comme valeur la position dans la grille de chaque lettre du mot cherché

            // maintenant qu'on a notre première lettre on cherche toutes les autres dans une seule et même boucle
            for (int j = 1; j < strlen(word); j++)
            {

                int position = is_letter_around_x_position(grid, square_matrix_grid_size, k, word[j], word_positions); // prends en compte la lettre d'avant et vérifie si la suivante est autour en retournant sa position

                // si position = -1 alors la méthode n'a pas trouvé la lettre suivante donc on sort de la boucle for
                if (position == -1)
                {
                    j = strlen(word);
                }

                word_positions[j] = position; // mets la position de la lettre trouvée dans le tableau de position
                k = position;                 // k prends la valeur de la position de la lettre récemment trouvée pour la prochaine itération de ce for

                if (j + 1 == strlen(word)) // vérifie si le programme a trouvé toutes les lettres du mot cherché auquel cas il retourne 1
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int is_letter_around_x_position(char *grid, int square_matrix_grid_size, int x, char letter, int *word_positions)
{

    // crée un tableau contenant toutes les positions possibles autour d'une lettre dans la grille selon les différents cas ainsi qu'une variable pour savoir jusqu'à quelle valeur on parcours le tableau de positions
    int *positions;
    int n = 0;

    if (x % square_matrix_grid_size == 0) // cas où la lettre est dans la première colonne
    {
        n = 5;
        positions = malloc(sizeof(int) * 5);
        positions[0] = x - square_matrix_grid_size;
        positions[1] = x - square_matrix_grid_size + 1;
        positions[2] = x + 1;
        positions[3] = x + square_matrix_grid_size;
        positions[4] = x + square_matrix_grid_size + 1;
    }
    else if (x % square_matrix_grid_size == square_matrix_grid_size - 1) // cas où la lettre est dans la dernière colonne
    {
        n = 5;
        positions = malloc(sizeof(int) * 5);
        positions[0] = x - square_matrix_grid_size - 1;
        positions[1] = x - square_matrix_grid_size;
        positions[2] = x - 1;
        positions[3] = x + square_matrix_grid_size - 1;
        positions[4] = x + square_matrix_grid_size;
    }
    else // tous les autres cas
    {
        n = 8;
        positions = malloc(sizeof(int) * 8);
        positions[0] = x - square_matrix_grid_size - 1;
        positions[1] = x - square_matrix_grid_size;
        positions[2] = x - square_matrix_grid_size + 1;
        positions[3] = x - 1;
        positions[4] = x + 1;
        positions[5] = x + square_matrix_grid_size - 1;
        positions[6] = x + square_matrix_grid_size;
        positions[7] = x + square_matrix_grid_size + 1;
    }

    for (int j = 0; j < n; j++)
    {
        if (positions[j] >= 0 && positions[j] < strlen(grid) && grid[positions[j]] == letter) // vérifie si la position séléctionnée est valide donc qu'elle ne sort pas de la grille (>= 0 et < taille de la grille) ce qui permet aussi de sécuriser les coins
        {
            if (!is_position_in_word_positions(word_positions, positions[j])) // vérifie si la position de la lettre trouvée n'est pas déjà dans le tableau de positions car sinon cela la comptabiliserait 2 fois
            {
                return positions[j]; // retourne la position de la lettre trouvée
            }
        }
    }

    return -1;
}

int is_position_in_word_positions(int *word_positions, int position)
{
    int length = sizeof(word_positions) / sizeof(int);

    // vérifie si la position entrée n'est pas déjà dans le tableau de positions afin de ne pas comptabiliser 2 fois la même lettre
    for (int i = 0; i < length; i++)
    {
        if (word_positions[i] == position)
        {
            return 1;
        }
    }
    return 0;
}

void modify_grid(char *grid, int word_length, int *word_positions)
{
    // mets en majuscule les lettres se trouvant aux positions rentrées dans le tableau de positions
    for (int i = 0; i < word_length; i++)
    {
        grid[word_positions[i]] = toupper(grid[word_positions[i]]);
    }
}