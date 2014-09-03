// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef widget_h
#define widget_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#include "../dsp/shared.hxx"
#include "avtk.h"
#include <stdio.h>
#include "header.c"
using namespace std;
#include <iostream>

class PandaWidget {
public:
  void update_button(int button);
  PandaWidget();
  Fl_Double_Window *window;
  Avtk::Image *headerImage;
private:
  void cb_headerImage_i(Avtk::Image*, void*);
  static void cb_headerImage(Avtk::Image*, void*);
public:
  Avtk::Compander *graph;
private:
  void cb_graph_i(Avtk::Compander*, void*);
  static void cb_graph(Avtk::Compander*, void*);
public:
  Avtk::Dial *factor;
private:
  void cb_factor_i(Avtk::Dial*, void*);
  static void cb_factor(Avtk::Dial*, void*);
public:
  Avtk::Dial *threshold;
private:
  void cb_threshold_i(Avtk::Dial*, void*);
  static void cb_threshold(Avtk::Dial*, void*);
public:
  Avtk::Dial *release;
private:
  void cb_release_i(Avtk::Dial*, void*);
  static void cb_release(Avtk::Dial*, void*);
public:
  Avtk::Dial *attack;
private:
  void cb_attack_i(Avtk::Dial*, void*);
  static void cb_attack(Avtk::Dial*, void*);
public:
  LV2UI_Write_Function write_function; 
  LV2UI_Controller controller; 
  void idle();
  int getWidth();
  int getHeight();
private:
  float gain; 
  float cutoff; 
  float Q; 
public:
  void writePort(int port, float& value);
  void close_cb(Fl_Widget* o, void*);
};
#endif
