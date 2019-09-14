#include "beat.hpp"

namespace Information {
  static const string Name        = "beat";
  static const string Version     = "v1.1";
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

auto showInformation(string_view text) -> void {
  MessageDialog().setTitle("beat").setAlignment(programWindow).setText(text).information();
}

auto showError(string_view text) -> void {
  MessageDialog().setTitle("beat").setAlignment(programWindow).setText(text).error();
}

auto askQuestion(string_view text, vector<string> questions) -> string {
  return MessageDialog().setTitle("beat").setAlignment(programWindow).setText(text).question(questions);
}

ApplyPatch::ApplyPatch() {
  setCollapsible();
  setVisible(false);

  header.setText("Apply BPS Patch").setFont(Font().setSize(16).setBold());

  patchHeader.setText("Step 1: choose the patch file to apply:");
  patchSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setFilters({{"BPS patches|*.bps"}, {"All files|*"}})
    .setPath(defaultPath)
    .setTitle("Select patch file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == originalLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    if(location && location == modifiedLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    patchLocation = location;
    synchronize();
  });
  patchLabel.setFont(Font().setBold());

  originalHeader.setText("Step 2: choose the original file to apply the patch to:");
  originalSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select original file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == patchLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    if(location && location == modifiedLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    originalLocation = location;
    synchronize();
  });
  originalLabel.setFont(Font().setBold());

  modifiedHeader.setText("Step 3: choose where to write the modified file to:");
  modifiedSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select modified file")
    .setAlignment(programWindow)
    .saveFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == patchLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    if(location && location == originalLocation) {
      location = {};
      overwriteOption.setChecked();
    }
    modifiedLocation = location;
    synchronize();
  });
  modifiedLabel.setFont(Font().setBold());

  overwriteOption.setText("Overwrite the original file (irreversible)").onToggle([&] {
    synchronize();
  });

  applyHeader.setText("Step 4: apply the patch:");
  applyButton.setText("Apply").onActivate([&] { apply(); });

  synchronize();
}

auto ApplyPatch::synchronize() -> void {
  patchLabel.setText(displayName(patchLocation));
  originalLabel.setText(displayName(originalLocation));
  if(!overwriteOption.checked()) {
    modifiedSelect.setEnabled(true);
    modifiedLabel.setText(displayName(modifiedLocation));
  } else {
    modifiedSelect.setEnabled(false);
    modifiedLabel.setText(displayName(originalLocation));
  }

  applyButton.setEnabled(patchLocation && originalLocation && (modifiedLocation || overwriteOption.checked()));
}

auto ApplyPatch::apply() -> void {
  auto patchSize = file::size(patchLocation);
  auto originalSize = file::size(originalLocation);

  if(patchSize > 64_MiB || originalSize > 64_MiB) showInformation({
    "Because this is a large patch, it will take a few seconds to apply.\n",
    Information::Name, " will be unresponsive until patching has completed.\n",
    "Please be patient."
  });

  programWindow.setTitle({"Applying Patch - ", Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(false);
  Application::processEvents();

  string manifest;
  string result;

  auto patchData = file::read(patchLocation);
  auto originalData = file::read(originalLocation);
  auto modifiedData = Beat::Single::apply(originalData, patchData, manifest, result);

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
      modifiedData.reset();
    }
  }

  if(modifiedData) {
    string location = overwriteOption.checked() ? originalLocation : modifiedLocation;
    file::write(location, *modifiedData);
    if(askQuestion({
      "Patch successfully applied.\n"
      "Continue performing another action, or quit the program?"
    }, {"Continue", "Quit"}) != "Continue") {
      return Application::quit();
    }
  }

  programWindow.setTitle({Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(true);
  patchLocation = {};
  originalLocation = {};
  modifiedLocation = {};
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

  patchHeader.setText("Step 1: choose a location to save the patch file to:");
  patchSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setFilters({{"BPS patches|*.bps"}, {"All files|*"}})
    .setName("output.bps")
    .setPath(defaultPath)
    .setTitle("Select patch file")
    .setAlignment(programWindow)
    .saveFile();
    if(location) defaultPath = Location::path(location);
    if(location && !location.endsWith(".bps")) {
      location.append(".bps");
    }
    if(location && location == originalLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    if(location && location == modifiedLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    patchLocation = location;
    synchronize();
  });
  patchLabel.setFont(Font().setBold());

  originalHeader.setText("Step 2: choose the original file to create a patch from:");
  originalSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select original file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == patchLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    if(location && location == modifiedLocation) {
      showError("This file was already chosen as the modified file.");
      location = {};
    }
    originalLocation = location;
    synchronize();
  });
  originalLabel.setFont(Font().setBold());

  modifiedHeader.setText("Step 3: choose the modified file to create a patch to:");
  modifiedSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setPath(defaultPath)
    .setTitle("Select modified file")
    .setAlignment(programWindow)
    .openFile();
    if(location) defaultPath = Location::path(location);
    if(location && location == patchLocation) {
      showError("This file was already chosen as the patch file.");
      location = {};
    }
    if(location && location == originalLocation) {
      showError("This file was already chosen as the original file.");
      location = {};
    }
    modifiedLocation = location;
    synchronize();
  });
  modifiedLabel.setFont(Font().setBold());

  createHeader.setText("Step 4: create the patch:");
  createButton.setText("Create").onActivate([&] { create(); });

  synchronize();
}

auto CreatePatch::synchronize() -> void {
  patchLabel.setText(displayName(patchLocation));
  originalLabel.setText(displayName(originalLocation));
  modifiedLabel.setText(displayName(modifiedLocation));

  createButton.setEnabled(patchLocation && originalLocation && modifiedLocation);
}

auto CreatePatch::create() -> void {
  auto originalSize = file::size(originalLocation);
  auto modifiedSize = file::size(modifiedLocation);
  if(originalSize >= 2_GiB || modifiedSize >= 2_GiB) return showError({
    "Sorry, beat cannot currently create patches against files larger than 2 gigabytes.\n"
    "Hopefully this support can be added in a future release."
  });
  auto minuteEstimate = round((originalSize + modifiedSize) / 500_KiB / 60.0 * 10.0) / 10.0;
  auto memoryEstimate = round((originalSize + modifiedSize) / 1_MiB * 17.0 * 10.0) / 10.0;
  if(askQuestion({
    "It will take approximately ", minuteEstimate, " minute(s) to create this patch.\n",
    "It will require approximately ", memoryEstimate, " megabyte(s) of RAM available to create this patch.\n",
    Information::Name, " will not be responsive while the patch is being created.\n",
    "Would you like to continue? Please be patient if so."
  }, {"Yes", "No"}) != "Yes") {
    return;
  }

  programWindow.setTitle({"Creating Patch - ", Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(false);
  Application::processEvents();

  auto originalData = file::read(originalLocation);
  auto modifiedData = file::read(modifiedLocation);
  auto patchData = Beat::Single::create(originalData, modifiedData);
  file::write(patchLocation, patchData);

  if(askQuestion({
    "Patch created successfully.\n"
    "Continue performing another action, or quit the program?"
  }, {"Continue", "Quit"}) != "Continue") {
    return Application::quit();
  }

  programWindow.setTitle({Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(true);
  patchLocation = {};
  originalLocation = {};
  modifiedLocation = {};
  synchronize();
}

Home::Home() {
  setCollapsible();
  setVisible(false);

  header.setText("beat").setFont(Font().setSize(16).setBold());
  subheader.setText({
    "GUI usage:\n"
    "  Choose an action from the left-hand panel.\n"
    "\n"
    "Command-line usage:\n"
    "  beat -apply:bps [-unsafe] {patch.bps} {original.file} [{modified.file}]\n"
    "  beat -create:bps {patch.bps} {original.file} {modified.file}"
  });
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
  panelList.append(ListViewItem().setText("Usage Instructions"));
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
    if(item.offset() == 2) home.setVisible();
  } else {
    home.setVisible();
  }
  panelContainer.resize();
}

#include <nall/main.hpp>
auto nall::main(Arguments arguments) -> void {
  if(arguments.take("-apply:bps")) {
    bool unsafe = arguments.take("-unsafe");

    string patchName = arguments.take();
    if(!file::exists(patchName)) return print("error: patch filename does not exist\n");
    if(!patchName.endsWith(".bps")) return print("error: patch filename must end with .bps\n");

    string originalName = arguments.take();
    if(!file::exists(originalName)) return print("error: original filename does not exist\n");
    if(originalName.endsWith(".bps")) return print("error: original filename must not end with .bps\n");

    string modifiedName = arguments.take();
    if(!modifiedName) modifiedName = originalName;
    if(modifiedName.endsWith(".bps")) return print("error: modified filename must not end with .bps\n");

    auto patchData = file::read(patchName);
    auto originalData = file::read(originalName);
    string manifest;
    string result;
    auto modifiedData = Beat::Single::apply(originalData, patchData, manifest, result);

    if(result && !unsafe) return print(result, "\n");
    if(result) print(result, "\n");
    if(!modifiedData) return;  //should never occur

    file::write(modifiedName, *modifiedData);
    return print("patch applied successfully\n");
  }

  if(arguments.take("-create:bps")) {
    string patchName = arguments.take();
    if(!patchName.endsWith(".bps")) return print("error: patch filename must end with .bps\n");

    string originalName = arguments.take();
    if(!file::exists(originalName)) return print("error: original filename does not exist\n");
    if(originalName.endsWith(".bps")) return print("error: original filename must not end with .bps\n");

    string modifiedName = arguments.take();
    if(!file::exists(modifiedName)) return print("error: modified filename does not exist\n");
    if(modifiedName.endsWith(".bps")) return print("error: modified filename must not end with .bps\n");

    if(originalName == modifiedName) return print("error: original and modified filenames cannot be the same\n");

    auto originalData = file::read(originalName);
    auto modifiedData = file::read(modifiedName);
    auto patchData = Beat::Single::create(originalData, modifiedData);

    file::write(patchName, patchData);
    return print("patch created successfully\n");
  }

  Application::setName("beat");
  Instances::programWindow.construct();
  Application::run();
  Instances::programWindow.destruct();
}
