#include <nall/nall.hpp>
#include <nall/beat/single/apply.hpp>
#include <nall/beat/single/create.hpp>
using namespace nall;

#include <hiro/hiro.hpp>
using namespace hiro;

struct ApplyPatch : VerticalLayout {
  ApplyPatch();
  auto synchronize() -> void;
  auto apply() -> void;

protected:
  string inputLocation;
  string sourceLocation;
  string targetLocation;

  Label header{this, Size{~0, 0}};
  Label inputHeader{this, Size{~0, 0}};
  HorizontalLayout inputLayout{this, Size{~0, 0}};
    Button inputSelect{&inputLayout, Size{80_sx, 0}};
    Label inputLabel{&inputLayout, Size{~0, 0}};
  Label sourceHeader{this, Size{~0, 0}};
  HorizontalLayout sourceLayout{this, Size{~0, 0}};
    Button sourceSelect{&sourceLayout, Size{80_sx, 0}};
    Label sourceLabel{&sourceLayout, Size{~0, 0}};
  Label targetHeader{this, Size{~0, 0}};
  HorizontalLayout targetLayout{this, Size{~0, 0}};
    Button targetSelect{&targetLayout, Size{80_sx, 0}};
    Label targetLabel{&targetLayout, Size{~0, 0}};
  CheckLabel overwriteOption{this, Size{~0, 0}};
  Label applyHeader{this, Size{~0, 0}};
  HorizontalLayout applyLayout{this, Size{~0, 0}};
    Button applyButton{&applyLayout, Size{80_sx, 0}};
};

struct ApplyPatches : VerticalLayout {
  ApplyPatches();

protected:
  Label header{this, Size{~0, 0}};
};

struct CreatePatch : VerticalLayout {
  CreatePatch();
  auto synchronize() -> void;
  auto create() -> void;

protected:
  string sourceLocation;
  string targetLocation;
  string outputLocation;

  Label header{this, Size{~0, 0}};
  Label sourceHeader{this, Size{~0, 0}};
  HorizontalLayout sourceLayout{this, Size{~0, 0}};
    Button sourceSelect{&sourceLayout, Size{80_sx, 0}};
    Label sourceLabel{&sourceLayout, Size{~0, 0}};
  Label targetHeader{this, Size{~0, 0}};
  HorizontalLayout targetLayout{this, Size{~0, 0}};
    Button targetSelect{&targetLayout, Size{80_sx, 0}};
    Label targetLabel{&targetLayout, Size{~0, 0}};
  Label outputHeader{this, Size{~0, 0}};
  HorizontalLayout outputLayout{this, Size{~0, 0}};
    Button outputSelect{&outputLayout, Size{80_sx, 0}};
    Label outputLabel{&outputLayout, Size{~0, 0}};
  Label createHeader{this, Size{~0, 0}};
  HorizontalLayout createLayout{this, Size{~0, 0}};
    Button createButton{&createLayout, Size{80_sx, 0}};
};

struct Home : VerticalLayout {
  Home();

protected:
  Label header{this, Size{~0, 0}};
  Label subheader{this, Size{~0, 0}};
  Button aboutButton{this, Size{80_sx, 0}};
};

struct ProgramWindow : Window {
  ProgramWindow();
  auto panelChange() -> void;

  ApplyPatch applyPatch;
  ApplyPatches applyPatches;
  CreatePatch createPatch;
  Home home;

  HorizontalLayout layout{this};
    ListView panelList{&layout, Size{150_sx, ~0}};
    VerticalLayout panelContainer{&layout, Size{~0, ~0}};
};
