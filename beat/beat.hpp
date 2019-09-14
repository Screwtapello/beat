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
  string patchLocation;
  string originalLocation;
  string modifiedLocation;

  Label header{this, Size{~0, 0}};

  Label patchHeader{this, Size{~0, 0}};
  HorizontalLayout patchLayout{this, Size{~0, 0}};
    Button patchSelect{&patchLayout, Size{80_sx, 0}};
    Label patchLabel{&patchLayout, Size{~0, 0}};

  Label originalHeader{this, Size{~0, 0}};
  HorizontalLayout originalLayout{this, Size{~0, 0}};
    Button originalSelect{&originalLayout, Size{80_sx, 0}};
    Label originalLabel{&originalLayout, Size{~0, 0}};

  Label modifiedHeader{this, Size{~0, 0}};
  HorizontalLayout modifiedLayout{this, Size{~0, 0}};
    Button modifiedSelect{&modifiedLayout, Size{80_sx, 0}};
    Label modifiedLabel{&modifiedLayout, Size{~0, 0}};
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
  string patchLocation;
  string originalLocation;
  string modifiedLocation;

  Label header{this, Size{~0, 0}};

  Label patchHeader{this, Size{~0, 0}};
  HorizontalLayout patchLayout{this, Size{~0, 0}};
    Button patchSelect{&patchLayout, Size{80_sx, 0}};
    Label patchLabel{&patchLayout, Size{~0, 0}};

  Label originalHeader{this, Size{~0, 0}};
  HorizontalLayout originalLayout{this, Size{~0, 0}};
    Button originalSelect{&originalLayout, Size{80_sx, 0}};
    Label originalLabel{&originalLayout, Size{~0, 0}};

  Label modifiedHeader{this, Size{~0, 0}};
  HorizontalLayout modifiedLayout{this, Size{~0, 0}};
    Button modifiedSelect{&modifiedLayout, Size{80_sx, 0}};
    Label modifiedLabel{&modifiedLayout, Size{~0, 0}};

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
