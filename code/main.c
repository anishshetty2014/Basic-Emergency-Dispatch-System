#include"functions.c"

float calculate_distance(float x1, float y1, float x2, float y2) {
    return sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

fib_heap* read_data_file(float x1, float y1) {
    FILE *file = fopen("safe_zones.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    fib_heap* f = (fib_heap*)malloc(sizeof(fib_heap));
    if (!f) {
        fclose(file);
        return NULL;
    }
    init_fib_heap(f);
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) <= 1) continue;
        int id;
        float x, y;
        if (sscanf(line, "%d,%f,%f", &id, &x, &y) == 3) insert_fib_heap(f, id, calculate_distance(x1, y1, x, y), x, y);
        else fprintf(stderr, "Error parsing line: %s", line);
    }

    fclose(file);
    return f;
}

int main() {
    printf("Menu:\n");
    printf("1. Enter emergency.\n");
    printf("2. Get Safe Zone.\n");
    printf("3. Insert Safe Zone.\n");
    printf("4. Delete Safe Zone.\n");
    printf("5. Print Treap and Fib heap of user.\n");
    printf("6. Quit.\n");
    
    int choice;
    treap t;
    init_treap(&t);
    int id_counter = 0;
    int timestamp = 0;
    
    while(1) {
        printf("Enter a choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1: {
                float x, y;
                int priority;
                printf("Enter your x coordinate: ");
                scanf("%f", &x);
                printf("Enter your y coordinate: ");
                scanf("%f", &y);
                printf("Enter the Priority [1,5]: ");
                scanf("%d", &priority);
                
                if (priority < 1 || priority > 5) {
                    printf("Invalid priority. Must be between 1 and 5.\n");
                    break;
                }
                
                if (id_counter >= MAX_ELEMENTS) {
                    printf("Maximum number of emergencies reached.\n");
                    break;
                }
                
                ARRAY[id_counter].x = x;
                ARRAY[id_counter].y = y;
                ARRAY[id_counter].f = read_data_file(x, y);
                
                if (insert_treap(&t, id_counter, priority, timestamp++) != 0) {
                    printf("Failed to insert emergency record.\n");
                    if (ARRAY[id_counter].f) free(ARRAY[id_counter].f);
                } 
                else {
                    printf("Emergency recorded with ID: %d\n", id_counter);
                    id_counter++;
                }
                break;
            }    
            case 2: {
                int user_id;
                printf("Enter your id: ");
                scanf("%d", &user_id);
                
                if (user_id < 0 || user_id >= id_counter) {
                    printf("Invalid ID.\n");
                    break;
                }
                
                if (ARRAY[user_id].f == NULL || ARRAY[user_id].f->min == NULL) {
                    printf("No safe zones available for this emergency.\n");
                    break;
                }
                
                fib_node* min_node = ARRAY[user_id].f->min;
                printf("Closest Safe Zone ID: %d\n", min_node->safe_id);
                printf("Coordinates: (%.2f, %.2f)\n", min_node->x, min_node->y); 
                printf("Distance: %.2f\n", min_node->distance);
                break;
            }  
            case 3: {
                int new_id;
                float new_x, new_y;
                printf("Enter new safe zone ID: ");
                scanf("%d", &new_id);
                printf("Enter x coordinate: ");
                scanf("%f", &new_x);
                printf("Enter y coordinate: ");
                scanf("%f", &new_y);
                
                FILE *file = fopen("safe_zones.txt", "a");
                if (file == NULL) {
                    perror("Error opening file");
                    break;
                }
                fprintf(file, "%d,%.1f,%.1f\n", new_id, new_x, new_y);
                fclose(file);
                printf("Safe zone added successfully.\n");
                break;
            }
            case 4: {
                int delete_id;
                printf("Enter safe zone ID to delete: ");
                scanf("%d", &delete_id);
                
                FILE *file = fopen("safe_zones.txt", "r");
                if (file == NULL) {
                    perror("Error opening file");
                    break;
                }
                
                FILE *temp = fopen("temp.txt", "w");
                if (temp == NULL) {
                    fclose(file);
                    perror("Error creating temporary file");
                    break;
                }
                
                char line[100];
                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    int current_id;
                    float x, y;
                    if (sscanf(line, "%d,%f,%f", &current_id, &x, &y) == 3) {
                        if (current_id != delete_id) fprintf(temp, "%d,%.1f,%.1f\n", current_id, x, y);
                        else found = 1;
                    } 
                    else fprintf(temp, "%s", line);
                }
                
                fclose(file);
                fclose(temp);
                
                remove("safe_zones.txt");
                rename("temp.txt", "safe_zones.txt");
                
                if (found) printf("Safe zone %d deleted successfully.\n", delete_id);
                else printf("Safe zone %d not found.\n", delete_id);

                break;
            }
            case 5: {
                printf("Enter user ID to display data: ");
                int display_id;
                scanf("%d", &display_id);
                
                if (display_id < 0 || display_id >= id_counter) {
                    printf("Invalid ID.\n");
                    break;
                }
                
                printf("\n=== Treap Structure ===\n");
                display_treap(t, 0);
                
                printf("\n=== Fibonacci Heap for User ID %d ===\n", display_id);
                if (ARRAY[display_id].f && ARRAY[display_id].f->head) {
                    display_fib_heap(ARRAY[display_id].f->head, 0, ARRAY[display_id].f->head);
                    printf("Min node: ID %d, Distance %.2f\n", 
                           ARRAY[display_id].f->min->safe_id, 
                           ARRAY[display_id].f->min->distance);
                } 
                else printf("No Fibonacci heap data for this user.\n");
                break;
            }   
            case 6:
                printf("Exiting program.\n");
                for (int i = 0; i < id_counter; i++) {
                    if (ARRAY[i].f) {
                        while (ARRAY[i].f->head) {
                            fib_node* extracted = extract_min(ARRAY[i].f);
                            if (extracted) free(extracted);
                        }
                        free(ARRAY[i].f);
                    }
                }
                while (t) remove_treap(&t, t->id);
                return 0;
                
            default:
                printf("Invalid Choice.\n");
        }
    }
    return 0;
}