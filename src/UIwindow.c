#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

// typedefs
typedef char* string;

// Global Vars
int WINDOW_WIDTH = 1980;
int WINDOW_HEIGHT = 1020;
int SIDEBAR_WIDTH;
int TEXT_INPUT_WIDTH;
int TEXT_INPUT_HEIGHT;
int CHAT_HEADER_HEIGHT;
bool show_cursor = 0;

struct message_container{
    string text;
    SDL_Texture* message;
    SDL_Rect msg;
} msg_container[1];

SDL_Surface* text_input_func(char* c, TTF_Font* fonty){
    SDL_Color fontColor = {255, 255, 255};
    SDL_Surface* text = TTF_RenderText_Solid(fonty, c , fontColor);

    return text;
}

int main(int argc, char** argv[]){
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if(SDL_Init (SDL_INIT_VIDEO) < 0){
        printf("SDL Failed : %s\n", SDL_GetError());
    }
    
    // window and renderer
    window = SDL_CreateWindow("Dappa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window,-1, SDL_RENDERER_ACCELERATED);
    
    // Text Props
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("/home/vignesh/dappa/resource/ZedMonoNerdFontMono-Bold.ttf", 24);
    SDL_Surface* textSurface;
    SDL_Texture* message = NULL;
    SDL_Surface* send_button_surface = text_input_func("Send", font);
    SDL_Texture* send_button = SDL_CreateTextureFromSurface(renderer, send_button_surface);
    SDL_Rect text_input_buffer;
    text_input_buffer.x = 450;
    text_input_buffer.y = 945;
    text_input_buffer.w = 0;
    text_input_buffer.h = 30;
    
    SDL_Rect sidebar = {400, 0, 1980, 1020};
    SDL_Rect text_input = {440, 940, 1440, 40};
    SDL_Rect chat_header = {400, 0, 1580, 100};
    SDL_Rect send_rect = {1785, 940, 85, 40};
    
    char text_typed[256] = "";

    // Main loop with event
    SDL_Event e;
    bool quit = false;
    while (quit == false) {
        while(SDL_PollEvent( &e )){
            //printf("Event : %d\n", e.wheel.direction);
            if(e.type == SDL_WINDOWEVENT){
                if(e.window.event == SDL_WINDOWEVENT_RESIZED){
                    SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
                }
            }
            else if(e.type == SDL_QUIT){
                quit = true;
            }
            else if(e.type == SDL_KEYDOWN && show_cursor){
                switch (e.key.keysym.sym)
                {
                    case SDLK_RETURN:
                        if(message) SDL_DestroyTexture(message);
                        if(textSurface) SDL_FreeSurface(textSurface);
                        textSurface = text_input_func(text_typed, font);
                        message = SDL_CreateTextureFromSurface(renderer, textSurface);
                        text_input_buffer.x += 1000;
                        text_input_buffer.y -= 50;
                        break;
                    case SDLK_BACKSPACE:
                        if(message) SDL_DestroyTexture(message);
                        if(textSurface) SDL_FreeSurface(textSurface);
                        strcpy(&text_typed[strlen(text_typed)], "");
                        printf("Text: %s\n", text_typed);
                        textSurface = text_input_func(text_typed, font);
                        message = SDL_CreateTextureFromSurface(renderer, textSurface);
                        text_input_buffer.w -= 24;
                        break;
                    default:
                        break;
                }
            }
            else if(e.type == SDL_TEXTINPUT && show_cursor){
                if(message) SDL_DestroyTexture(message);
                if(textSurface) SDL_FreeSurface(textSurface);
                strcpy(&text_typed[strlen(text_typed)], e.text.text);
                textSurface = text_input_func(text_typed, font);
                message = SDL_CreateTextureFromSurface(renderer, textSurface);
                text_input_buffer.w += 24;
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN){
                switch (e.button.clicks)
                {
                    case SDL_BUTTON_LMASK:
                        printf("Click coords : %d,%d\n", e.button.x, e.button.y);
                        if(e.button.x >= text_input_buffer.x && e.button.y > 946 && e.button.y < 976) show_cursor = 1;
                        else show_cursor = 0;
                        break;
                
                    default:
                        break;
                }
            }
        }

        // Layout bounds
        SIDEBAR_WIDTH = (int) WINDOW_WIDTH * 20 / 100;
        TEXT_INPUT_WIDTH = (int) SIDEBAR_WIDTH;
        TEXT_INPUT_HEIGHT = (int) WINDOW_HEIGHT * 96 / 100;
        CHAT_HEADER_HEIGHT = (int) WINDOW_HEIGHT * 10 /100;
        
        // Set BG to near black to clear screen
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        if(message) SDL_RenderCopy(renderer, message, NULL, &text_input_buffer);
        
        SDL_RenderDrawRect(renderer, &sidebar);
        SDL_Rect sb_chat_list = {0, 0, 400, 90};
        for(int i = 0; i < 10; i++){
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &sb_chat_list);
            sb_chat_list.y += 95;
        }
        SDL_RenderDrawRect(renderer, &text_input);
        SDL_RenderDrawRect(renderer, &chat_header);
        if(show_cursor) SDL_RenderDrawLine(renderer, text_input_buffer.x + text_input_buffer.w + 5, 946, text_input_buffer.x + text_input_buffer.w + 5, 976);
        SDL_RenderDrawLine(renderer, 1780, 940, 1780, 980);
        SDL_RenderCopy(renderer, send_button, NULL, &send_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(message);
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
