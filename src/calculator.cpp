#include <iostream>
#include <SDL2/SDL.h>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <bitset>
#include <SDL2/SDL_ttf.h>

typedef struct FileEntry{
    std::string filename;
    bool isDir;
}FileEntry;

typedef struct Rectangles{
    std::vector<int> x;
    std::vector<int> y;
    std::vector<int> w;
    std::vector<int> h;
    std::vector<char> actions;
}Rectangles;

void EventLoop(SDL_Renderer *renderer, int *mouseX, int *mouseY, bool *pressed, Rectangles *rects, float *result);
void Render(SDL_Renderer *renderer, Rectangles *rects, float *result);
SDL_Texture* RasterizeText(std::string text, std::string font_file, int font_size, SDL_Color color, SDL_Renderer *renderer);
bool PointInRectangle(int *mouseX, int *mouseY, int rectX, int rectY, int rectW, int rectH);


int main(int argc, char **argv)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error: could not initialize SLD2\n");
        exit(1);
    }
    
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    if(SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer) != 0)
    {
        fprintf(stderr, "Error: could not create window\n");
        exit(1);
    }
    
    TTF_Init();
    
    int mouseX = 0;
    int mouseY = 0;
    bool pressed = false;
    Rectangles rects;

    float result = 0.0;

    Render(renderer, &rects, &result);
    EventLoop(renderer, &mouseX, &mouseY, &pressed, &rects, &result);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}


void EventLoop(SDL_Renderer *renderer, int *mouseX, int *mouseY, bool *pressed, Rectangles *rects, float *result)
{
    bool running = true;
    SDL_Event event;
    
    while(running)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                running = false;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    *pressed = true;
                    for(int i=0; i<rects->x.size(); i++)
                    {
                        if(PointInRectangle(mouseX, mouseY, rects->x[i], rects->y[i], rects->w[i], rects->h[i]) )
                        {
                            //*path = *path + "/" + rects->actio[i];
                            Render(renderer, rects, result);
                        }
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                *mouseX = event.motion.x;
                *mouseY = event.motion.y;
                break;
        }
    }
}

void Render(SDL_Renderer *renderer, Rectangles *rects, float *result)
{
	//fill the background
    SDL_SetRenderDrawColor(renderer, 170, 185, 196, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    //dark grey
    SDL_SetRenderDrawColor(renderer, 67, 76, 91, SDL_ALPHA_OPAQUE);

    //stuff for the text
    uint32_t format;
    int access, width, height;
    SDL_Texture *name;

    //white for the buttons
    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;


    //start off with the result bar
    SDL_Rect resultBar;
    resultBar.x = 20;
    resultBar.y = 15;
    resultBar.w = 750;
    resultBar.h = 100;
    SDL_RenderFillRect(renderer, &resultBar);
    name = RasterizeText(std::to_string(*result), "./OpenSans/OpenSans-Regular.ttf", 25, color, renderer);
    SDL_QueryTexture(name, &format, &access, &width, &height);
    SDL_Rect resultText;
    resultText.x = 50;
    resultText.y = 50;
    resultText.w = width;
    resultText.h = height;
    SDL_RenderCopy(renderer, name, NULL, &resultText);
    SDL_DestroyTexture(name);


    SDL_Rect button;
    int buttonX = 20;
    int buttonY = 130;
    button.w = 100;
    button.h = 100;
    
    SDL_Rect number;
    int numberX = 60;
    int numberY = 170;


    std::string values;
    values = "987X654/321+0C-=";
    int pos = 0;
    for(int i=0; i<4; i++)
    {
    	for(int j=0; j<4; j++)
    	{
    		//draw icon
		    button.y = buttonY;
		    button.x = buttonX;
		    SDL_RenderFillRect(renderer, &button);
		    
		    //draw number in icon
		    name = RasterizeText(std::string(1,values[pos]), "./OpenSans/OpenSans-Regular.ttf", 25, color, renderer);
		    SDL_QueryTexture(name, &format, &access, &width, &height);
		    number.x = numberX;
		    number.y = numberY;
		    number.w = width;
		    number.h = height;
		    SDL_RenderCopy(renderer, name, NULL, &number);
		    SDL_DestroyTexture(name);

		    //push onto rectangles
		    rects->x.push_back(button.x);
            rects->y.push_back(button.y);
            rects->w.push_back(button.w);
            rects->h.push_back(button.h);
            rects->actions.push_back(values[pos]);

		    //increments
		    pos++;
		    buttonX = buttonX + 120;
		    numberX = numberX + 120; 
    	}

    	buttonX = 20;
    	numberX = 60;
    	buttonY = buttonY + 120;
		numberY = numberY + 120; 

    }  
    SDL_RenderPresent(renderer);
    

}


SDL_Texture* RasterizeText(std::string text, std::string font_file, int font_size, SDL_Color color, SDL_Renderer *renderer)
{
    TTF_Font *font = TTF_OpenFont(font_file.c_str(), font_size);
    if(font == NULL)
    {
        fprintf(stderr, "Error: could not open font\n");
        return NULL;
    }
    
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if(surface == NULL)
    {
        fprintf(stderr, "Error: could not render text to surface\n");
        return NULL;
    }
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(texture == NULL)
    {
        fprintf(stderr, "Error: could not convert surface to texture\n");
    }
    
    SDL_FreeSurface(surface);
    return texture;
}

bool PointInRectangle(int *mouseX, int *mouseY, int rectX, int rectY, int rectW, int rectH)
{
    if(*mouseX >= rectX && *mouseX <= (rectX + rectW) )
    {
        if(*mouseY > rectY && *mouseY <= (rectY + rectH) )
        {
            return true;
        }
    }   
    return false;
}





