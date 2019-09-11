#include "beat.hpp"

namespace Information {
  static const string Name        = "beat";
  static const string Version     = "v0";
  static const string Author      = "byuu";
  static const string License     = "GPLv3";
  static const string Website     = "https://byuu.org";
  static const string Description = "A binary patching tool using the beat file format.";
}

namespace Instances { Instance<ProgramWindow> programWindow; }
ProgramWindow& programWindow = Instances::programWindow();

ApplyPatch::ApplyPatch() {
  setCollapsible();
  setVisible(false);

  header.setText("Apply BPS Patch").setFont(Font().setSize(16).setBold());

  inputHeader.setText("Step 1: choose the patch file to apply:");
  inputSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setTitle("Select patch file")
    .setAlignment(programWindow)
    .setFilters({{"BPS patches|*.bps"}, {"All files|*"}})
    .openFile();
    if(location && location == sourceLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the original file.").error();
      location = {};
    }
    if(location && location == targetLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the modified file.").error();
      location = {};
    }
    if(location) {
      inputLocation = location;
      inputLabel.setText({Location::file(location), " (", Location::path(location).trimRight("/", 1L), ")"});
    } else {
      inputLocation = {};
      inputLabel.setText("(no file selected)");
    }
    synchronize();
  });
  inputLabel.setText("(no file selected)").setFont(Font().setBold());

  sourceHeader.setText("Step 2: choose the original file to apply the patch to:");
  sourceSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setTitle("Select original file")
    .setAlignment(programWindow)
    .openFile();
    if(location && location == inputLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the patch file.").error();
      location = {};
    }
    if(location && location == targetLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the modified file.").error();
      location = {};
    }
    if(location) {
      sourceLocation = location;
      sourceLabel.setText({Location::file(location), " (", Location::path(location).trimRight("/", 1L), ")"});
    } else {
      sourceLocation = {};
      sourceLabel.setText("(no file selected)");
    }
    synchronize();
  });
  sourceLabel.setText("(no file selected)").setFont(Font().setBold());

  targetHeader.setText("Step 3: choose where to write the modified file to:");
  targetSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setTitle("Select modified file")
    .setAlignment(programWindow)
    .saveFile();
    if(location && location == inputLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the patch file.").error();
      location = {};
    }
    if(location && location == sourceLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the original file.").error();
      location = {};
    }
    if(location) {
      targetLocation = location;
      targetLabel.setText({Location::file(location), " (", Location::path(location).trimRight("/", 1L), ")"});
    } else {
      targetLocation = {};
      targetLabel.setText("(no file selected)");
    }
    synchronize();
  });
  targetLabel.setText("(no file selected)").setFont(Font().setBold());

  applyHeader.setText("Step 4: apply the patch:");
  applyButton.setText("Apply").onActivate([&] { apply(); });

  synchronize();
}

auto ApplyPatch::synchronize() -> void {
  applyButton.setEnabled(inputLocation && sourceLocation && targetLocation);
}

auto ApplyPatch::apply() -> void {
  programWindow.setTitle({"Applying Patch - ", Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(false);

  string manifest;
  string result;

  auto input = file::read(inputLocation);
  auto source = file::read(sourceLocation);
  auto target = Beat::Single::apply(source, input, manifest, result);

  if(target) {
    file::write(targetLocation, *target);
    if(MessageDialog().setAlignment(programWindow).setText(
      "Patch applied successfully.\n"
      "Continue performing another action, or quit the program?"
    ).question({"Continue", "Quit"}) != "Continue") {
      return Application::quit();
    }
  } else {
    MessageDialog().setAlignment(programWindow).setText({
      "Patching failed.\n"
      "Error: ", result
    }).error();
  }

  programWindow.setTitle({Information::Name, " ", Information::Version});
  programWindow.layout.setEnabled(true);
  inputLocation = {};
  sourceLocation = {};
  targetLocation = {};
  inputLabel.setText("(no file selected)");
  sourceLabel.setText("(no file selected)");
  targetLabel.setText("(no file selected)");
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
    .setTitle("Select original file")
    .setAlignment(programWindow)
    .openFile();
    if(location && location == targetLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the modified file.").error();
      location = {};
    }
    if(location && location == outputLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the patch file.").error();
      location = {};
    }
    if(location) {
      sourceLocation = location;
      sourceLabel.setText({Location::file(location), " (", Location::path(location).trimRight("/", 1L), ")"});
    } else {
      sourceLocation = {};
      sourceLabel.setText("(no file selected)");
    }
    synchronize();
  });
  sourceLabel.setText("(no file selected)").setFont(Font().setBold());

  targetHeader.setText("Step 2: choose the modified file to create a patch to:");
  targetSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setTitle("Select modified file")
    .setAlignment(programWindow)
    .openFile();
    if(location && location == sourceLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the original file.").error();
      location = {};
    }
    if(location && location == outputLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the patch file.").error();
      location = {};
    }
    if(location) {
      targetLocation = location;
      targetLabel.setText({Location::file(location), " (", Location::path(location).trimRight("/", 1L), ")"});
    } else {
      targetLocation = {};
      targetLabel.setText("(no file selected)");
    }
    synchronize();
  });
  targetLabel.setText("(no file selected)").setFont(Font().setBold());

  outputHeader.setText("Step 3: choose a location to save the patch file to:");
  outputSelect.setText("Select").onActivate([&] {
    string location = BrowserDialog()
    .setTitle("Select patch file")
    .setAlignment(programWindow)
    .setFilters({{"BPS patches|*.bps"}, {"All files|*"}})
    .saveFile();
    if(location && !location.endsWith(".bps")) {
      location.append(".bps");
    }
    if(location && location == sourceLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the original file.").error();
      location = {};
    }
    if(location && location == targetLocation) {
      MessageDialog().setAlignment(programWindow).setText("This file was already chosen as the modified file.").error();
      location = {};
    }
    if(location) {
      outputLocation = location;
      outputLabel.setText({Location::file(location), " (", Location::path(location).trimRight("/", 1L), ")"});
    } else {
      outputLocation = {};
      outputLabel.setText("(no file selected)");
    }
    synchronize();
  });
  outputLabel.setText("(no file selected)").setFont(Font().setBold());

  createHeader.setText("Step 4: create the patch:");
  createButton.setText("Create").onActivate([&] { create(); });

  synchronize();
}

auto CreatePatch::synchronize() -> void {
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
  sourceLabel.setText("(no file selected)");
  targetLabel.setText("(no file selected)");
  outputLabel.setText("(no file selected)");
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
  panelList.append(ListViewItem().setText("Apply Patch").setIcon(Icon::Emblem::Text));
//panelList.append(ListViewItem().setText("Apply Patches").setIcon(Icon::Edit::Copy));
  panelList.append(ListViewItem().setText("Create Patch").setIcon(Icon::Action::Save));
  panelList.item(0).setSelected();
  panelList.doChange();

  panelContainer.append(applyPatch, Size{~0, ~0});
  panelContainer.append(applyPatches, Size{~0, ~0});
  panelContainer.append(createPatch, Size{~0, ~0});
  panelContainer.append(home, Size{~0, ~0});

  setTitle({Information::Name, " ", Information::Version});
  setSize({640_sx, 400_sy});
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
