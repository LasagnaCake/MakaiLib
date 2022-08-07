// #include <irrlicht.h>
#include <stdexcept>

#include "lib/program.hpp"

#define _SIDE_A
#ifdef _SIDE_A
#include <windows.h>

#ifndef _DEBUG_OUTPUT_
#if (_WIN32 || _WIN64)
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
#endif // _WINDOWS
#else
int main() {
#endif // _MAKAI_DEBUG_
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*/
	//Makai::Program prog(640, 480, "OK");
	//Makai::Program prog(720, 640, "OK");
	Makai::Program prog(960, 720, "OK");
	//Makai::Program prog(1280, 960, "OK");
	prog.windowColor = Vector::Vector4(Vector::Vector3(.5), 1);
	Vector::VecV3 verts[3] = {
		Vector::VecV3(100, 100, 0),
		Vector::VecV3(100, 200, 0),
		Vector::VecV3(200, 200, 0)
	};
	$debug("Triangulating...");
	RenderData::Triangle3D triangle(verts);
	triangle.color[0] = Vector::VecV4(1);
	triangle.color[1] = Vector::VecV4(1);
	triangle.color[2] = Vector::VecV4(1);
	ALLEGRO_VERTEX* vtxs = RenderData::asPrimitives(triangle, false);
	prog.onDraw = $func() {
		al_draw_text(prog.defaultFont, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
		RenderData::render3DStrip(vtxs, 3, NULL, false);
	};
	$debug("And fire!");
	prog.run();
	return 0;
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    must_init(al_init_image_addon(), "image addon");
    ALLEGRO_BITMAP* mysha = al_load_bitmap("river.jpg");
    must_init(mysha, "mysha");

    must_init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // game logic goes here.
                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
			$debug("De riveur...");
            al_draw_bitmap(mysha, 100, 100, 0);
			$debug("Sacre bleu!");
			$debug("La line...");
			al_draw_line(440, 110, 460, 210, al_map_rgb_f(1, 0, 0), 1);
			$debug("Hon hon hon!");
            al_draw_line(500, 220, 570, 200, al_map_rgb_f(1, 1, 0), 1);
            $debug("Deux!");
            $debug("Lé triangleu...");
            al_draw_filled_triangle(35, 350, 85, 375, 35, 400, al_map_rgb_f(0, 1, 0));
            $debug("Mi amore!");
            $debug("Lé bricktozefáce...");
            al_draw_filled_rectangle(240, 260, 340, 340, al_map_rgba_f(0, 0, 0.5, 0.5));
            $debug("Rezendevoux!");
            $debug("Lé rounde...");
            al_draw_circle(450, 370, 30, al_map_rgb_f(1, 0, 1), 2);
			$debug("Euclideux!");
			$debug("Lé hatrede fore huméneteies...");
            ALLEGRO_VERTEX v[] = {
                { .x = 210, .y = 320, .z = 0, .color = al_map_rgb_f(1, 0, 0) },
                { .x = 330, .y = 320, .z = 0, .color = al_map_rgb_f(0, 1, 0) },
                { .x = 210, .y = 420, .z = 0, .color = al_map_rgb_f(0, 0, 1) },
                { .x = 330, .y = 420, .z = 0, .color = al_map_rgb_f(1, 1, 0) },
            };
            al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
			$debug("Mirácle!");
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_bitmap(mysha);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

#endif // _SIDE_A
