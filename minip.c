#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define height 20
#define width 50
#define maxshape 100

typedef enum{
    line,
    rectangle,
    triangle,
    circle
}Shapetype;

typedef struct{
    Shapetype type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int width, height;
    int radius;
}Shape;

char canvas[height][width];
shape histroy[maxshape];
int shapecount = 0;

void initialcanvas()
{
    for(int i=0;i<height;i++)
    {
         for(int j=0;j<width;j++){
            canvas[i][j]='_';
         }
    }
}

void displaycanvas(){
    system("clear");
    printf("~2D GRAPHICS EDITOR CANVAS~\n\n");
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            printf("%c",canvas[i][j]);
        }
        printf("\n");
    }
    print("\n_____________________________________________\n");
}

int main(){
    int choice;
    intitialcanvas();
    do{
        displaycanvas();
        printf("Main menu:\n");
        printf("1. Add an object\n");
        printf("2. Delete an object\n");
        printf("3. Modify an object\n");
        printf("4. Exit\n");
        printf("Enter your chice(1-4): ");
        scanf("%d", &choice);
        
        switch(choice){
            int x,y,w,h;
            case 1:
            printf("Add object selected\n");
            printf("Press Enter to continue");
            getchar(); getchar();
            break;
            case 2:
            printf("Delete object selected\n");
            printf("Press Enter to continue");
            getchar(); getchar();
            break;
            case 3:
            printf("Modify object selected\n");
            printf("Press Enter to continue");
            getchar(); getchar();
            break;
            case 4:
            printf("Exiting program. Goodbye!");
            break;
            default:
            printf("Invalid Choice! Please try again.\n");
            printf("Press Enter to continue...");
            getchar(); getchar();
        }    
    }while (choice!=4);
    return 0;
}
