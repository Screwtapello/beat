#include "beat.hpp"

namespace Information {
  static const string Name        = "beat";
  static const string Version     = "v0.1";
  static const string Author      = "byuu";
  static const string License     = "GPLv3";
  static const string Website     = "https://byuu.org";
  static const string Description = "A binary patching tool using the beat file format.";
}

namespace Instances { Instance<ProgramWindow> programWindow; }
ProgramWindow& programWindow = Instances::programWindow();
string defaultPath = Path::desktop();

auto displayName(string_view name) -> string {
  if(!name) return "(no file selected)";
  return {Location::file(name), " (", Location::path(name).trimRight("/", 1L), ")"};
}

auto showError(string_view text) -> void {
  MessageDialog().setAlignment(programWindow).setText(text).error();
}

auto askQuestion(string_view text, vector<string> questions) -> string {
  return MessageDialog().setAlignment(programWindow).setText(text).question(questions);
}

ApplyPatch::ApplyPatch() {
  setCollapsible();
  setVisible(false);

  header.setText("Apply BPS Patch").setFont(Font().setSize(16).setBold());

  inputHeader.setText("Step 1: choose the patch file to apply:");
  inputSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setFilters({{"BPS patches|*.bps"}, {"All files|*"}})
    .setPath(defaultPath)
    .setTitle("Select patch file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == sourceLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    if(location && location == targetLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    inputLocation = location;
    synchronize();
  });
  inputLabel.setFont(Font().setBold());

  sourceHeader.setText("Step 2: choose the original file to apply the patch to:");
  sourceSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select original file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == inputLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    if(location && location == targetLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    sourceLocation = location;
    synchronize();
  });
  sourceLabel.setFont(Font().setBold());

  targetHeader.setText("Step 3: choose where to write the modified file to:");
  targetSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select modified file")
    .setAlignment(programWindow)
    .saveFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == inputLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    if(location && location == sourceLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    targetLocation = location;
    synchronize();
  });
  targetLabel.setFont(Font().setBold());

  overwriteOption.setText("Overwrite the original file (irreversible)").onToggle([&] {
    synchronize();
  });

  applyHeader.setText("Step 4: apply the patch:");
  applyButton.setText("Apply").onActivate([&] { apply(); });

  synchronize();
}

auto ApplyPatch::synchronize() -> void {
  inputLabel.setText(displayName(inputLocation));
  sourceLabel.setText(displayName(sourceLocation));
  if(!overwriteOption.checked()) {
    targetSelect.setEnabled(true);
    targetLabel.setText(displayName(targetLocation));
  } else {
    targetSelect.setEnabled(false);
    targetLabel.setText(displayName(sourceLocation));
  }

  applyButton.setEnabled(inputLocation && sourceLocation && (targetLocation || overwriteOption.checked()));
}

auto ApplyPatch::apply() -> void {
  programWindow.setTitle({"Applying Patch - ", Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(false);

  string manifest;
  string result;

  auto input = file::read(inputLocation);
  auto source = file::read(sourceLocation);
  auto target = Beat::Single::apply(source, input, manifest, result);

  if(result.beginsWith("error: ")) {
    result.trimLeft("error: ", 1L);
    showError({"Patching failed with the error: ", result, "."});
  }

  if(result.beginsWith("warning: ")) {
    result.trimLeft("warning: ", 1L);
    if(askQuestion({
      "Patch applied, but with the warning: ", result, ".\n",
      "The output is likely to be invalid. Keep the result anyway?"
    }, {"Keep", "Discard"}) != "Keep") {
      target.reset();
    }
  }

  if(target) {
    string location = overwriteOption.checked() ? sourceLocation : targetLocation;
    file::write(location, *target);
    if(askQuestion({
      "Patch applied.\n"
      "Continue performing another action, or quit the program?"
    }, {"Continue", "Quit"}) != "Continue") {
      return Application::quit();
    }
  }

  programWindow.setTitle({Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(true);
  inputLocation = {};
  sourceLocation = {};
  targetLocation = {};
  overwriteOption.setChecked(false);
  synchronize();
}

ApplyPatches::ApplyPatches() {
  setCollapsible();
  setVisible(false);

  header.setText("Apply Multiple BPS Patches").setFont(Font().setSize(16).setBold());
}

CreatePatch::CreatePatch() {
  setCollapsible();
  setVisible(false);

  header.setText("Create BPS Patch").setFont(Font().setSize(16).setBold());

  sourceHeader.setText("Step 1: choose the original file to create a patch from:");
  sourceSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select original file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == targetLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    if(location && location == outputLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    sourceLocation = location;
    synchronize();
  });
  sourceLabel.setFont(Font().setBold());

  targetHeader.setText("Step 2: choose the modified file to create a patch to:");
  targetSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select modified file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == sourceLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    if(location && location == outputLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    targetLocation = location;
    synchronize();
  });
  targetLabel.setFont(Font().setBold());

  outputHeader.setText("Step 3: choose a location to save the patch file to:");
  outputSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setFilters({{"BPS patches|*.bps"}, {"All files|*"}})
    .setPath(defaultPath)
    .setTitle("Select patch file")
    .setAlignment(programWindow)
    .saveFile();
    if(location) defaultPath = Location::path(location);
    if(location && !location.endsWith(".bps")) {
      location.append(".bps");
    }
    if(location && location == sourceLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    if(location && location == targetLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    outputLocation = location;
    synchronize();
  });
  outputLabel.setFont(Font().setBold());

  createHeader.setText("Step 4: create the patch:");
  createButton.setText("Create").onActivate([&] { create(); });

  synchronize();
}

auto CreatePatch::synchronize() -> void {
  sourceLabel.setText(displayName(sourceLocation));
  targetLabel.setText(displayName(targetLocation));
  outputLabel.setText(displayName(outputLocation));

  createButton.setEnabled(sourceLocation && targetLocation && outputLocation);
}

auto CreatePatch::create() -> void {
  programWindow.setTitle({"Creating Patch - ", Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(false);
  auto source = file::read(sourceLocation);
  auto target = file::read(targetLocation);
  auto output = Beat::Single::create(source, target);
  file::write(outputLocation, output);

  if(MessageDialog().setAlignment(programWindow).setText(
    "Patch created successfully.\n"
    "Continue performing another action, or quit the program?"
  ).question({"Continue", "Quit"}) != "Continue") {
    return Application::quit();
  }

  programWindow.setTitle({Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(true);
  sourceLocation = {};
  targetLocation = {};
  outputLocation = {};
  synchronize();
}

Home::Home() {
  setCollapsible();
  setVisible(false);

  header.setText("beat").setFont(Font().setSize(16).setBold());
  subheader.setText("Please choose an action from the left-hand panel.");
  aboutButton.setText("About").onActivate([&] {
    AboutDialog()
    .setName(Information::Name)
    .setDescription(Information::Description)
    .setVersion(Information::Version)
    .setAuthor(Information::Author)
    .setLicense(Information::License)
    .setWebsite(Information::Website)
    .setAlignment(programWindow)
    .show();
  });
}

ProgramWindow::ProgramWindow() {
  onClose(&Application::quit);
  layout.setPadding(5_sx, 5_sy);

  panelList.onChange([&] { panelChange(); });
  panelList.append(ListViewItem().setText("Apply Patch"));
//panelList.append(ListViewItem().setText("Apply Patches"));
  panelList.append(ListViewItem().setText("Create Patch"));
  panelList.item(0).setSelected();
  panelList.doChange();

  panelContainer.append(applyPatch, Size{~0, ~0});
  panelContainer.append(applyPatches, Size{~0, ~0});
  panelContainer.append(createPatch, Size{~0, ~0});
  panelContainer.append(home, Size{~0, ~0});

  setTitle({Information::Name, " ", Information::Version});
  setSize({640_sx, 360_sy});
  setAlignment(Alignment::Center);
  setVisible();
}

auto ProgramWindow::panelChange() -> void {
  applyPatch.setVisible(false);
  applyPatches.setVisible(false);
  createPatch.setVisible(false);
  home.setVisible(false);
  if(auto item = panelList.selected()) {
    if(item.offset() == 0) applyPatch.setVisible();
  //if(item.offset() == 1) applyPatches.setVisible();
    if(item.offset() == 1) createPatch.setVisible();
  } else {
    home.setVisible();
  }
  panelContainer.resize();
}

#include <nall/main.hpp>
auto nall::main(Arguments arguments) -> void {
  Application::setName("beat");
  Instances::programWindow.construct();
  Application::run();
  Instances::programWindow.destruct();
}
