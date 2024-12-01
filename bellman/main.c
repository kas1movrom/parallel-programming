#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <readline/readline.h>
#include <time.h>

#define eof -1
#define START_CAPACITY 50
#define MAX_EDGES 4
#define sep " "

const char *graph_msgs[] = {
        "Successfully!\n",
        "Duplicate point!\n",
        "Vertices not close!\n",
        "Cant find vertex\n",
        "Cant found path\n"
};

enum VertexType {
    ENTER,
    EXIT,
    CONNECT
};

enum FuncResult {
    OKAY,
    DUPLICATE,
    VERTICES_NOT_CLOSE,
    NOT_FOUND_VERTEX,
    NOT_FOUND_PATH
};

enum Bool {
    FALSE,
    TRUE
};


typedef struct Bellman {
    int pred;
    int distance;
    int weight;
}
Bellman;

typedef struct Edge {
    int from;
    int to;
    int weight;
}
Edge;

typedef struct Vertex {
    int x;
    int y;
    enum VertexType type;

	int count;
    int capacity; // change
    Edge *edges;
}
Vertex;

typedef struct Graph {
    int capacity;
    int size;
    Vertex *vertices;
}
Graph;


Graph *create_graph()
{
    Graph *graph = (Graph *) malloc(sizeof(Graph));
    graph->size = 0;
    graph->capacity = START_CAPACITY;

    graph->vertices = (Vertex *) malloc(sizeof(Vertex) * START_CAPACITY);
    return graph;
}

void destroy_graph(Graph *graph)
{
	if (graph == NULL)
		return;
		
    for (int i = 0; i < graph->size; i++) {
        free(graph->vertices[i].edges);
    }
    free(graph->vertices);
    free(graph);
}

int find_vertex(Graph *graph, int x, int y)
{
    for (int i = 0; i < graph->size; ++i) {
        if (graph->vertices[i].x == x && graph->vertices[i].y == y)
            return i;
    }
    return -1;
}

void init_vertex(Graph *graph, int x, int y, enum VertexType type)
{
    graph->vertices[graph->size].count = 0;
    graph->vertices[graph->size].x = x;
    graph->vertices[graph->size].y = y;

    graph->vertices[graph->size].type = type;
    graph->vertices[graph->size].edges = (Edge*) malloc(sizeof(Edge) * MAX_EDGES);
    graph->vertices[graph->size].capacity = MAX_EDGES;
}

enum FuncResult add_vertex(Graph *graph, int x, int y, enum VertexType type)
{
    if (find_vertex(graph, x, y) != -1)
        return DUPLICATE;

    if (graph->size == graph->capacity)
    {
        graph->capacity *= 2;
        graph->vertices = (Vertex *) realloc(graph->vertices, sizeof(Vertex) * graph->capacity);
    }
    init_vertex(graph, x, y, type);

    ++graph->size;
    return OKAY;
}

char *get_line(FILE *file)
{
	char *ptr = (char*)malloc(sizeof(char));
	char buf[81];
	int n, len = 0;
	*ptr = '\0';
	do {
		n = fscanf(file, "%80[^\n]", buf);
		if (n < 0)
		{
			free(ptr);
			ptr = NULL;
			continue;
		}
		if (n == 0)
		{
			fscanf(file, "%*c");
		}
		if (n > 0)
		{
			len += (int)strlen(buf);
			ptr = (char*)realloc(ptr, len + 1);
			strcat(ptr, buf);
		}
	} while(n > 0);
	return ptr;
}

enum Bool check_vertex_on_close(int x1, int y1, int x2, int y2)
{
    return ((x1 == x2) && abs(y1 - y2) == 1) || ((y1 == y2) && abs(x1 - x2) == 1);
}

void edge_connection(Graph *graph, int index1, int index2, int weight) // change
{
    if (graph->vertices[index1].count == graph->vertices[index1].capacity)
    {
        graph->vertices[index1].capacity *= 2;
        graph->vertices[index1].edges = (Edge *) realloc(graph->vertices[index1].edges, sizeof(Edge) * graph->vertices[index1].capacity);
    }

    int count = ++graph->vertices[index1].count;

    graph->vertices[index1].edges[count - 1].from = index1;
    graph->vertices[index1].edges[count - 1].to = index2;
    graph->vertices[index1].edges[count - 1].weight = weight;
}

enum FuncResult add_edge(Graph *graph, int x1, int y1, int x2, int y2, int weight) // change
{
    // if (check_vertex_on_close(x1, y1, x2, y2) == FALSE)
    //     return VERTICES_NOT_CLOSE;

    int index1, index2;
    if ((index1 = find_vertex(graph, x1, y1)) == -1 || (index2 = find_vertex(graph, x2, y2)) == -1)
        return NOT_FOUND_VERTEX;

    for (int i = 0; i < graph->vertices[index1].count; ++i) {
        if (graph->vertices[index1].edges[i].to == index2)
            return DUPLICATE;
    }

    edge_connection(graph, index1, index2, weight);
    return OKAY;
}

enum FuncResult change_weight(Graph *graph, int x1, int y1, int x2, int y2, int weight) // add function
{
    // if (check_vertex_on_close(x1, y1, x2, y2) == FALSE)
    //     return VERTICES_NOT_CLOSE;

    int index1, index2;
    if ((index1 = find_vertex(graph, x1, y1)) == -1 || (index2 = find_vertex(graph, x2, y2)) == -1)
        return NOT_FOUND_VERTEX;

    for (int i = 0; i < graph->vertices[index1].count; ++i) {
        if (graph->vertices[index1].edges[i].to == index2) {
            graph->vertices[index1].edges[i].weight = weight;
            return OKAY;
        }
    }
    return NOT_FOUND_PATH;
}


enum FuncResult import_from_file(char* filename, Graph *graph) // change
{
    FILE* file = fopen(filename, "r");

    char *line = get_line(file);
    while (line != NULL)
    {
        int array[3], cnt = 0;
        char *token = strtok(line, sep);
		while (token != NULL)
		{
			array[cnt] = atoi(token);
			++cnt;
			token = strtok(NULL, sep);
		}

        add_vertex(graph, array[0], array[1], array[2]);

        free(line);
        line = get_line(file);
    }
    for (int i = 0; i < graph->size; ++i) {
        for (int j = i + 1; j < graph->size; ++j) {
            add_edge(graph, graph->vertices[j].x, graph->vertices[j].y, graph->vertices[i].x, graph->vertices[i].y, 1);
            add_edge(graph, graph->vertices[i].x, graph->vertices[i].y, graph->vertices[j].x, graph->vertices[j].y, 1);
        }
    }

    return OKAY;
}

void print_vertex(Vertex vertex)
{
    printf("{%d, %d, ", vertex.x, vertex.y);
    if (vertex.type == ENTER)
        printf("ENTER}");

    if (vertex.type == EXIT)
        printf("EXIT}");

    if (vertex.type == CONNECT)
        printf("CONNECT}");
}

void print_edges(Graph *graph, Vertex vertex) // change
{
    print_vertex(vertex);
    printf(" --> ");
    for (int i = 0; i < vertex.count; ++i) {
        print_vertex(graph->vertices[vertex.edges[i].to]);
        printf("--{%d} ", vertex.edges[i].weight);
    }
    printf("\n");
}

void print_graph(Graph *graph)
{
    for (int i = 0; i < graph->size; ++i)
        print_edges(graph, graph->vertices[i]);
}

void correct_edge(Graph *graph, int j, int i) // change
{
    for (int k = j; k < graph->vertices[i].count - 1; ++k) {
        graph->vertices[i].edges[k].to = graph->vertices[i].edges[k + 1].to;
        graph->vertices[i].edges[k].from = graph->vertices[i].edges[k + 1].from;
        graph->vertices[i].edges[k].weight = graph->vertices[i].edges[k + 1].weight;
    }
    --graph->vertices[i].count;
}

void correct_vertices(Graph* graph, int index)
{
    graph->vertices[index].edges = graph->vertices[index + 1].edges;
    graph->vertices[index].x = graph->vertices[index + 1].x;
    graph->vertices[index].y = graph->vertices[index + 1].y;

    graph->vertices[index].capacity = graph->vertices[index + 1].capacity;
    graph->vertices[index].type = graph->vertices[index + 1].type;
    graph->vertices[index].count = graph->vertices[index + 1].count;
}

enum FuncResult delete_vertex(Graph *graph, int x, int y)
{
    int index;
    if ((index = find_vertex(graph, x, y)) == -1)
        return NOT_FOUND_VERTEX;

    free(graph->vertices[index].edges);
    for (int i = 0; i < graph->size; ++i) {
        if (i == index)
            continue;

        for (int j = 0; j < graph->vertices[i].count; ++j) {
            if (graph->vertices[i].edges[j].to == index) {
                correct_edge(graph, j, i);
                break;
            }
        }
        for (int j = 0; j < graph->vertices[i].count; ++j) {
            if (graph->vertices[i].edges[j].to > index)
                --graph->vertices[i].edges[j].to;

            if (graph->vertices[i].edges[j].from > index)
                --graph->vertices[i].edges[j].from;
        }
    }
    for (int i = index; i < graph->size - 1; ++i)
        correct_vertices(graph, i);

    graph->vertices[graph->size - 1].edges = NULL;
    --graph->size;
    return OKAY;
}

enum FuncResult delete_edge(Graph *graph, int x1, int y1, int x2, int y2)
{
    // if (check_vertex_on_close(x1, y1, x2, y2) == FALSE)
    //     return VERTICES_NOT_CLOSE;

    int index1, index2;
    if ((index1 = find_vertex(graph, x1, y1)) == -1 || (index2 = find_vertex(graph, x2, y2)) == -1)
        return NOT_FOUND_VERTEX;

    for (int i = 0; i < graph->vertices[index1].count; ++i) {
        if (graph->vertices[index1].edges[i].to == index2) {
            correct_edge(graph, i, index1);
            return OKAY;
        }
    }
    return NOT_FOUND_PATH;
}

enum FuncResult change_type(Graph *graph, int x, int y, enum VertexType type)
{
    int index;
    if ((index = find_vertex(graph, x, y)) == -1)
        return NOT_FOUND_VERTEX;

    graph->vertices[index].type = type;
    return OKAY;
}

Bellman *bellman_ford(Graph* graph, int x1, int y1, int x2, int y2, int* index) // change
{
    int index1, index2;
    if ((index1 = find_vertex(graph, x1, y1)) == -1 || (index2 = find_vertex(graph, x2, y2)) == -1 ||
            graph->vertices[index1].type != ENTER || graph->vertices[index2].type != EXIT)
        return NULL;

    *index = index2;
    Bellman *array = (Bellman *) malloc(sizeof(Bellman) * graph->size);
    for (int i = 0; i < graph->size; ++i) {
        array[i].distance = INT_MAX / 2 - 1;
        array[i].pred = -1;
    }
    array[index1].distance = 0;

    for (int i = 1; i <= graph->size; ++i) {
        for (int j = 0; j < graph->size; ++j) {
            for (int k = 0; k < graph->vertices[j].count; ++k) {
                if (array[graph->vertices[j].edges[k].to].distance > array[j].distance + graph->vertices[j].edges[k].weight) {
                    array[graph->vertices[j].edges[k].to].distance = array[j].distance + graph->vertices[j].edges[k].weight;
                    array[graph->vertices[j].edges[k].to].pred = j;
                    array[graph->vertices[j].edges[k].to].weight = graph->vertices[j].edges[k].weight;
                }
            }
        }
    }
    if (array[index2].pred == -1) {
        free(array);
        return NULL;
    }
    
    for (int i = 0; i < graph->size; ++i) {
        for (int j  = 0; j < graph->vertices[i].count; ++j) {
            if (array[graph->vertices[i].edges[j].to].distance > array[i].distance + graph->vertices[i].edges[j].weight) {
                return NULL;
            }
        }
    }
    return array;
}

int get_positive_int(int *value)
{
    int n;
    while ((n = scanf("%d", value)) != 1 || *value <= 0)
    {
        if (n == EOF)
        {
            return -1;
        }
        printf("Enter positive int value!\n");
        scanf("%*c");
    }
    return 1;
}

int get_int(int* value) // add function
{
    int n;
    while ((n = scanf("%d", value)) != 1 || *value == 0)
    {
        if (n == EOF)
        {
            return -1;
        }
        printf("Enter positive int value!\n");
        scanf("%*c");
    }
    return 1;
}

int get_number_type(int *type)
{
    int n;
    while ((n = scanf("%d", type)) != 1 || *type < 0 || *type > 2)
    {
        if (n == EOF)
        {
            return -1;
        }
        printf("Enter the correct number of release!\n");
        scanf("%*c");
    }
    return 1;
}

int dialog(const char *msgs[], int N)
{
    char *errmsg = "";
    int rc;
    int i, n;
    do
    {
        puts(errmsg);
        errmsg = "Choose the correct number of alternatives!";

        for(i = 0; i < N; ++i)
            puts(msgs[i]);
        puts("Choose showed alternatives -->");
        while ((n = scanf("%d", &rc)) != 1 || rc < 0 || rc > 10)
        {
            if (n == EOF)
            {
                rc = 0;
                break;
            }
            printf("\n%s\n", errmsg);
            n = scanf("%*c");
        }
        if(n == 0)
            rc = 0;
    } while(rc < 0 || rc >= N);
    return rc;
}

int dialog_add_vertex(Graph *graph)
{
    int x, y, check1, check2;
    int type;

    printf("Enter point coordinates:\n>");
    check1 = get_positive_int(&x);
    if (check1 == -1)
        return eof;

    printf(">");
    check2 = get_positive_int(&y);
    if (check2 == eof)
        return eof;

    printf("Enter vertex type:\n\n");
    printf("0. Enter\n1. Exit\n2. Connect\n");
    check1 = get_number_type(&type);
    if (check1 == -1)
        return eof;

    printf("%s", graph_msgs[add_vertex(graph, x, y, type)]);
    return 1;
}

int dialog_add_edge(Graph *graph) // change
{
    int x1, y1, x2, y2, weight, check;
    printf("Enter first point coordinates:\n>");
    check = get_positive_int(&x1);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y1);
    if (check == eof)
        return eof;

    printf("Enter second point coordinates:\n>");
    check = get_positive_int(&x2);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y2);
    if (check == eof)
        return eof;

    printf("Enter weight of edge:\n>");
    check = get_int(&weight);
    if (check == eof)
        return eof;

    printf("%s", graph_msgs[add_edge(graph, x1, y1, x2, y2, weight)]);
    return 1;
}

int dialog_delete_vertex(Graph *graph)
{
    int x, y, check1, check2;

    printf("Enter point coordinates:\n>");
    check1 = get_positive_int(&x);
    if (check1 == -1)
        return eof;

    printf(">");
    check2 = get_positive_int(&y);
    if (check2 == eof)
        return eof;

    printf("%s", graph_msgs[delete_vertex(graph, x, y)]);
    return 1;
}

int dialog_delete_edge(Graph *graph)
{
    int x1, y1, x2, y2, check;
    printf("Enter first point coordinates:\n>");
    check = get_positive_int(&x1);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y1);
    if (check == eof)
        return eof;

    printf("Enter second point coordinates:\n>");
    check = get_positive_int(&x2);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y2);
    if (check == eof)
        return eof;

    printf("%s", graph_msgs[delete_edge(graph, x1, y1, x2, y2)]);
    return 1;
}

int dialog_change_type(Graph *graph)
{
    int x, y, check1, check2;
    int type;

    printf("Enter point coordinates:\n>");
    check1 = get_positive_int(&x);
    if (check1 == -1)
        return eof;

    printf(">");
    check2 = get_positive_int(&y);
    if (check2 == eof)
        return eof;

    printf("Enter vertex type:\n\n");
    printf("0. Enter\n1. Exit\n2. Connect\n");
    check1 = get_number_type(&type);
    if (check1 == -1)
        return eof;

    printf("%s", graph_msgs[change_type(graph, x, y, type)]);
    return 1;
}

int dialog_print_graph(Graph *graph)
{
    printf("Yor graph:\n");
    print_graph(graph);
    return 1;
}

int dialog_Bellman(Graph *graph) // change
{
    int x1, y1, x2, y2, check;
    printf("Enter first point coordinates:\n>");
    check = get_positive_int(&x1);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y1);
    if (check == eof)
        return eof;

    printf("Enter second point coordinates:\n>");
    check = get_positive_int(&x2);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y2);
    if (check == eof)
        return eof;

    int index = 0;
    Bellman *array = bellman_ford(graph, x1, y1, x2, y2, &index);
    if (array == NULL)
    {
        printf("%s", graph_msgs[4]);
        return 0;
    }

    int pred = index, length = 0;
    printf ("Your path:\n");
    while (1)
    {
        print_vertex(graph->vertices[pred]);
        int prev_weight = array[pred].weight;
        
        pred = array[pred].pred;
        if (pred == -1)
            break;

        length += prev_weight;
        printf("-{%d}", prev_weight);
        printf(" <-- ");
    }
    printf("\nlength : %d\n", length);
    free(array);
    return 1;
}

int dialog_change_weight(Graph* graph) // add function
{
    int x1, y1, x2, y2, weight, check;
    printf("Enter first point coordinates:\n>");
    check = get_positive_int(&x1);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y1);
    if (check == eof)
        return eof;

    printf("Enter second point coordinates:\n>");
    check = get_positive_int(&x2);
    if (check == -1)
        return eof;

    printf(">");
    check = get_positive_int(&y2);
    if (check == eof)
        return eof;

    printf("Enter new weight of edge:\n>");
    check = get_int(&weight);
    if (check == eof)
        return eof;

    printf("%s", graph_msgs[change_weight(graph, x1, y1, x2, y2, weight)]);
    return 1;
}

typedef int (*t_func) (Graph *);

int main(int argc, char** argv) 
{
    if (argc != 2) 
    {
        printf("Use ./main filename\n");
        return 1;
    }

    const char *msgs[] = {"0. Quit", "1. Add vertex", "2. Add edge",
                           "3. Delete vertex", "4. Delete edge",
                           "5. Change vertex type", "6. Print graph", 
                           "7. Finding the shortest path", "8. Change weight"};

    const int cnt_msgs = sizeof(msgs) / sizeof(msgs[0]);
    
    Graph *graph = create_graph();
    import_from_file(argv[1], graph);

    t_func func[9] = {NULL, dialog_add_vertex, dialog_add_edge,
                        dialog_delete_vertex, dialog_delete_edge,
                        dialog_change_type, dialog_print_graph,
                        dialog_Bellman, dialog_change_weight};

    int rc;
    while ((rc = dialog(msgs, cnt_msgs)) != 0)
    {
        int temp = func[rc](graph);
        if (temp == -1)
            break;
    }
    destroy_graph(graph);
    return 0;
}