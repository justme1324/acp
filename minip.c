#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define HEIGHT 100
#define WIDTH 100
#define MAX_SHAPES 100

typedef enum {
    LINE,
    RECTANGLE,
    TRIANGLE,
    CIRCLE
} Shapetype;

typedef struct {
    Shapetype type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int width, height;
    int radius;
    int fill; // 0 for Outline, 1 for Filled
} Shape;

char canvas[HEIGHT][WIDTH];
Shape history[MAX_SHAPES];
int shapecount = 0;

/* Helper to read an integer with boundary checks and clean input buffers */
int read_int(const char* prompt, int min_val, int max_val) {
    int val;
    int c;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                while ((c = getchar()) != '\n' && c != EOF);
                return val;
            }
            printf("Error: Input must be between %d and %d.\n", min_val, max_val);
            while ((c = getchar()) != '\n' && c != EOF);
        } else {
            printf("Error: Invalid input. Please enter an integer.\n");
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

/* Initialize the canvas with underscores */
void initialcanvas() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            canvas[i][j] = '_';
        }
    }
}

/* Safe pixel plotting with bounds check */
void plot_point(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

/* Bresenham's Line Algorithm */
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        plot_point(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/* Draw Rectangle (Outline or Filled) */
void draw_rectangle(int x1, int y1, int w, int h, int fill) {
    if (fill) {
        for (int y = y1; y < y1 + h; y++) {
            for (int x = x1; x < x1 + w; x++) {
                plot_point(x, y);
            }
        }
    } else {
        // Top and bottom edges
        for (int x = x1; x < x1 + w; x++) {
            plot_point(x, y1);
            plot_point(x, y1 + h - 1);
        }
        // Left and right edges
        for (int y = y1; y < y1 + h; y++) {
            plot_point(x1, y);
            plot_point(x1 + w - 1, y);
        }
    }
}

/* Draw Triangle (Outline or Filled) using Scanline Rasterization */
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int fill) {
    if (fill) {
        // Sort vertices by y-coordinate (y1 <= y2 <= y3)
        if (y1 > y2) { int tx = x1, ty = y1; x1 = x2; y1 = y2; x2 = tx; y2 = ty; }
        if (y1 > y3) { int tx = x1, ty = y1; x1 = x3; y1 = y3; x3 = tx; y3 = ty; }
        if (y2 > y3) { int tx = x2, ty = y2; x2 = x3; y2 = y3; x3 = tx; y3 = ty; }

        if (y3 == y1) return; // Degenerate flat triangle

        int total_height = y3 - y1;
        for (int i = 0; i < total_height; i++) {
            int curr_y = y1 + i;
            int second_half = curr_y > y2 || y2 == y1;
            int segment_height = second_half ? (y3 - y2) : (y2 - y1);
            if (segment_height == 0) continue;

            float alpha = (float)i / total_height;
            float beta  = (float)(curr_y - (second_half ? y2 : y1)) / segment_height;

            int ax = x1 + (int)((x3 - x1) * alpha);
            int bx = second_half ? (x2 + (int)((x3 - x2) * beta)) : (x1 + (int)((x2 - x1) * beta));

            if (ax > bx) { int t = ax; ax = bx; bx = t; }
            for (int x = ax; x <= bx; x++) {
                plot_point(x, curr_y);
            }
        }
        // Draw boundaries to ensure edges are fully closed and clean
        draw_line(x1, y1, x2, y2);
        draw_line(x2, y2, x3, y3);
        draw_line(x3, y3, x1, y1);
    } else {
        draw_line(x1, y1, x2, y2);
        draw_line(x2, y2, x3, y3);
        draw_line(x3, y3, x1, y1);
    }
}

/* Helper for Midpoint Circle Symmetry Plotting */
void plot_circle_points(int cx, int cy, int x, int y) {
    plot_point(cx + x, cy + y);
    plot_point(cx - x, cy + y);
    plot_point(cx + x, cy - y);
    plot_point(cx - x, cy - y);
    plot_point(cx + y, cy + x);
    plot_point(cx - y, cy + x);
    plot_point(cx + y, cy - x);
    plot_point(cx - y, cy - x);
}

/* Draw Circle (Outline or Filled) */
void draw_circle(int xc, int yc, int r, int fill) {
    int x = 0;
    int y = r;
    int p = 1 - r;

    if (fill) {
        // Draw horizontal lines between symmetry points
        for (int i = xc - x; i <= xc + x; i++) { plot_point(i, yc + y); plot_point(i, yc - y); }
        for (int i = xc - y; i <= xc + y; i++) { plot_point(i, yc + x); plot_point(i, yc - x); }

        while (x < y) {
            x++;
            if (p < 0) {
                p += 2 * x + 1;
            } else {
                y--;
                p += 2 * (x - y) + 1;
            }
            for (int i = xc - x; i <= xc + x; i++) { plot_point(i, yc + y); plot_point(i, yc - y); }
            for (int i = xc - y; i <= xc + y; i++) { plot_point(i, yc + x); plot_point(i, yc - x); }
        }
    } else {
        plot_circle_points(xc, yc, x, y);
        while (x < y) {
            x++;
            if (p < 0) {
                p += 2 * x + 1;
            } else {
                y--;
                p += 2 * (x - y) + 1;
            }
            plot_circle_points(xc, yc, x, y);
        }
    }
}

/* Redraw all shapes from history on a clean canvas */
void redraw_canvas() {
    initialcanvas();
    for (int i = 0; i < shapecount; i++) {
        Shape s = history[i];
        switch (s.type) {
            case LINE:
                draw_line(s.x1, s.y1, s.x2, s.y2);
                break;
            case RECTANGLE:
                draw_rectangle(s.x1, s.y1, s.width, s.height, s.fill);
                break;
            case TRIANGLE:
                draw_triangle(s.x1, s.y1, s.x2, s.y2, s.x3, s.y3, s.fill);
                break;
            case CIRCLE:
                draw_circle(s.x1, s.y1, s.radius, s.fill);
                break;
        }
    }
}

/* Display the canvas with nice frame borders and index helper labels */
void displaycanvas() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    printf("~ 2D GRAPHICS EDITOR CANVAS ~\n\n");
    
    // Top border
    printf("   ┌");
    for (int j = 0; j < WIDTH * 2; j++) printf("─");
    printf("┐\n");
    
    // Rows
    for (int i = 0; i < HEIGHT; i++) {
        printf("%2d │", i);
        for (int j = 0; j < WIDTH; j++) {
            printf("%c%c", canvas[i][j], canvas[i][j]);
        }
        printf("│\n");
    }
    
    // Bottom border
    printf("   └");
    for (int j = 0; j < WIDTH * 2; j++) printf("─");
    printf("┘\n");

    // X indices
    printf("    ");
    for (int j = 0; j < WIDTH; j += 10) {
        if (j % 10 == 0) {
            printf("%02d  ", j);
        } else {
            printf("    ");
        }
    }
    printf("\n\n");
}

/* Prints shape details for listing */
void print_shape(int index, Shape s) {
    printf("%d. ", index + 1);
    switch (s.type) {
        case LINE:
            printf("Line from (%d, %d) to (%d, %d)\n", s.x1, s.y1, s.x2, s.y2);
            break;
        case RECTANGLE:
            printf("%s Rectangle at (%d, %d), width: %d, height: %d\n", 
                   s.fill ? "Filled" : "Outline", s.x1, s.y1, s.width, s.height);
            break;
        case TRIANGLE:
            printf("%s Triangle with vertices (%d, %d), (%d, %d), (%d, %d)\n", 
                   s.fill ? "Filled" : "Outline", s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
            break;
        case CIRCLE:
            printf("%s Circle at (%d, %d), radius: %d\n", 
                   s.fill ? "Filled" : "Outline", s.x1, s.y1, s.radius);
            break;
    }
}

/* Menu to select a shape type and prompt user for coordinates */
void add_shape_menu() {
    if (shapecount >= MAX_SHAPES) {
        printf("Error: Canvas history is full. Cannot add more shapes.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    printf("\n--- Add an Object ---\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Triangle\n");
    printf("4. Circle\n");
    int type_choice = read_int("Select shape type (1-4): ", 1, 4);
    
    Shape new_shape;
    new_shape.fill = 0;
    
    if (type_choice == 1) {
        new_shape.type = LINE;
        printf("\nEntering Line parameters:\n");
        new_shape.x1 = read_int("Enter x1 (0-99): ", 0, WIDTH - 1);
        new_shape.y1 = read_int("Enter y1 (0-99): ", 0, HEIGHT - 1);
        new_shape.x2 = read_int("Enter x2 (0-99): ", 0, WIDTH - 1);
        new_shape.y2 = read_int("Enter y2 (0-99): ", 0, HEIGHT - 1);
    } else if (type_choice == 2) {
        new_shape.type = RECTANGLE;
        printf("\nEntering Rectangle parameters:\n");
        new_shape.x1 = read_int("Enter top-left x (0-99): ", 0, WIDTH - 1);
        new_shape.y1 = read_int("Enter top-left y (0-99): ", 0, HEIGHT - 1);
        new_shape.width = read_int("Enter width (1-100): ", 1, WIDTH);
        new_shape.height = read_int("Enter height (1-100): ", 1, HEIGHT);
        int fill_choice = read_int("Select style (1. Outline, 2. Filled): ", 1, 2);
        new_shape.fill = (fill_choice == 2);
    } else if (type_choice == 3) {
        new_shape.type = TRIANGLE;
        printf("\nEntering Triangle parameters:\n");
        new_shape.x1 = read_int("Enter x1 (0-99): ", 0, WIDTH - 1);
        new_shape.y1 = read_int("Enter y1 (0-99): ", 0, HEIGHT - 1);
        new_shape.x2 = read_int("Enter x2 (0-99): ", 0, WIDTH - 1);
        new_shape.y2 = read_int("Enter y2 (0-99): ", 0, HEIGHT - 1);
        new_shape.x3 = read_int("Enter x3 (0-99): ", 0, WIDTH - 1);
        new_shape.y3 = read_int("Enter y3 (0-99): ", 0, HEIGHT - 1);
        int fill_choice = read_int("Select style (1. Outline, 2. Filled): ", 1, 2);
        new_shape.fill = (fill_choice == 2);
    } else if (type_choice == 4) {
        new_shape.type = CIRCLE;
        printf("\nEntering Circle parameters:\n");
        new_shape.x1 = read_int("Enter center x (0-99): ", 0, WIDTH - 1);
        new_shape.y1 = read_int("Enter center y (0-99): ", 0, HEIGHT - 1);
        new_shape.radius = read_int("Enter radius (0-50): ", 0, 50);
        int fill_choice = read_int("Select style (1. Outline, 2. Filled): ", 1, 2);
        new_shape.fill = (fill_choice == 2);
    }
    
    history[shapecount++] = new_shape;
    redraw_canvas();
    printf("Shape added successfully!\n");
    printf("Press Enter to continue...");
    getchar();
}

/* Menu to delete an existing shape */
void delete_shape_menu() {
    if (shapecount == 0) {
        printf("\nNo shapes to delete.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    printf("\n--- Delete an Object ---\n");
    for (int i = 0; i < shapecount; i++) {
        print_shape(i, history[i]);
    }
    
    int index_to_delete = read_int("Enter the index of the shape to delete: ", 1, shapecount);
    
    for (int i = index_to_delete - 1; i < shapecount - 1; i++) {
        history[i] = history[i + 1];
    }
    shapecount--;
    
    redraw_canvas();
    printf("Shape deleted successfully!\n");
    printf("Press Enter to continue...");
    getchar();
}

/* Menu to modify parameters of an existing shape */
void modify_shape_menu() {
    if (shapecount == 0) {
        printf("\nNo shapes to modify.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    
    printf("\n--- Modify an Object ---\n");
    for (int i = 0; i < shapecount; i++) {
        print_shape(i, history[i]);
    }
    
    int index_to_modify = read_int("Enter the index of the shape to modify: ", 1, shapecount);
    int idx = index_to_modify - 1;
    
    printf("\nModifying shape ");
    print_shape(idx, history[idx]);
    
    if (history[idx].type == LINE) {
        history[idx].x1 = read_int("Enter new x1 (0-99): ", 0, WIDTH - 1);
        history[idx].y1 = read_int("Enter new y1 (0-99): ", 0, HEIGHT - 1);
        history[idx].x2 = read_int("Enter new x2 (0-99): ", 0, WIDTH - 1);
        history[idx].y2 = read_int("Enter new y2 (0-99): ", 0, HEIGHT - 1);
    } else if (history[idx].type == RECTANGLE) {
        history[idx].x1 = read_int("Enter new top-left x (0-99): ", 0, WIDTH - 1);
        history[idx].y1 = read_int("Enter new top-left y (0-99): ", 0, HEIGHT - 1);
        history[idx].width = read_int("Enter new width (1-100): ", 1, WIDTH);
        history[idx].height = read_int("Enter new height (1-100): ", 1, HEIGHT);
        int fill_choice = read_int("Select style (1. Outline, 2. Filled): ", 1, 2);
        history[idx].fill = (fill_choice == 2);
    } else if (history[idx].type == TRIANGLE) {
        history[idx].x1 = read_int("Enter new x1 (0-99): ", 0, WIDTH - 1);
        history[idx].y1 = read_int("Enter new y1 (0-99): ", 0, HEIGHT - 1);
        history[idx].x2 = read_int("Enter new x2 (0-99): ", 0, WIDTH - 1);
        history[idx].y2 = read_int("Enter new y2 (0-99): ", 0, HEIGHT - 1);
        history[idx].x3 = read_int("Enter new x3 (0-99): ", 0, WIDTH - 1);
        history[idx].y3 = read_int("Enter new y3 (0-99): ", 0, HEIGHT - 1);
        int fill_choice = read_int("Select style (1. Outline, 2. Filled): ", 1, 2);
        history[idx].fill = (fill_choice == 2);
    } else if (history[idx].type == CIRCLE) {
        history[idx].x1 = read_int("Enter new center x (0-99): ", 0, WIDTH - 1);
        history[idx].y1 = read_int("Enter new center y (0-99): ", 0, HEIGHT - 1);
        history[idx].radius = read_int("Enter new radius (0-50): ", 0, 50);
        int fill_choice = read_int("Select style (1. Outline, 2. Filled): ", 1, 2);
        history[idx].fill = (fill_choice == 2);
    }
    
    redraw_canvas();
    printf("Shape modified successfully!\n");
    printf("Press Enter to continue...");
    getchar();
}

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

    int choice;
    initialcanvas();
    do {
        displaycanvas();
        printf("Main menu:\n");
        printf("1. Add an object\n");
        printf("2. Delete an object\n");
        printf("3. Modify an object\n");
        printf("4. Exit\n");
        
        choice = read_int("Enter your choice (1-4): ", 1, 4);
        
        switch (choice) {
            case 1:
                add_shape_menu();
                break;
            case 2:
                delete_shape_menu();
                break;
            case 3:
                modify_shape_menu();
                break;
            case 4:
                printf("\nExiting program. Goodbye!\n");
                break;
        }    
    } while (choice != 4);
    
    return 0;
}
