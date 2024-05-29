function test() {
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickItem_0/n=checkableButton_0', true); // Button text: Simple Button
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickItem_0/n=checkableButton_0', false); // Button text: Simple Button
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/n=firstCheckBox_0', true); // Button text: First CheckBox
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/n=secondCheckBox_0', true); // Button text: Second CheckBox
    QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0', 'LeftButton', 642, 55);
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/n=secondCheckBox_0', false); // Button text: Second CheckBox
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_1/n=firstRadioButton_0'); // Button text: 'First RadioButton'
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_1/n=secondRadioButton_0'); // Button text: 'Second RadioButton'
    QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0', 'LeftButton', 642, 95);
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_2/n=simpleSwitch_0', true); // Button text: Switch
    QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0', 'LeftButton', 384, 135);
    QtAda.checkButton('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_2/n=simpleSwitch_0', false); // Button text: Switch
    QtAda.setDelayProgress('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_2/n=delayButton_0', 0.244333);
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_3/n=simpleSlider_0', 0.498433);
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_3/n=rangeSlider_0', 0.141066, 1);
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_3/n=rangeSlider_0', 0.141066, 0.855799);
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_3/n=simpleDial_0', 0.2);
    // Looks like focus click on TextInput
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=editableComboBox_0/c=QQuickTextField_0', 'LeftButton', 126, 15);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=editableComboBox_0', 'BananaD');
    QtAda.changeValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=simpleSpinBox_0', 'Up');
    // Looks like focus click on TextInput
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=simpleSpinBox_0/c=QQuickTextInput_0', 'LeftButton', 14, 9);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=simpleSpinBox_0', '68');
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=simpleSpinBox_0', 68);
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=doubleSpinBox_0', 3);
    QtAda.changeValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_4/n=doubleSpinBox_0', 'Down');
    QtAda.setValue('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyClickPage_0/c=QQuickColumnLayout_0/c=QQuickItem_2/n=simpleScrollBar_0', 0.5);
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickTabBar_0/n=textItemsTabButton_0');
    // Looks like focus click on TextInput
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_0/n=simpleTextInput_0', 'LeftButton', 126, 15);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_0/n=simpleTextInput_0', 'Original TextInDput');
    // Looks like focus click on TextInput
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_1/n=customTextInput_0', 'LeftButton', 126, 15);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_1/n=customTextInput_0', 'Custom TextIn7put');
    // Looks like focus click on TextInput
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_2/n=simpleTextField_0', 'LeftButton', 126, 15);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_2/n=simpleTextField_0', 'Orignal TextFieldD');
    // Looks like focus click on TextEdit
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_1/c=QQuickRectangle_0/n=simpleTextEdit_0', 'LeftButton', 190, 45);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_1/c=QQuickRectangle_0/n=simpleTextEdit_0', 'Orignal TextEdit7');
    // Looks like focus click on TextEdit
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_1/c=QQuickRectangle_1/n=simpleTextArea_0', 'LeftButton', 190, 45);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_1/c=QQuickRectangle_1/n=simpleTextArea_0', 'Orignal TextAreaD');
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickTabBar_0/n=viewItemsTabButton_0');
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyViewPage_0/c=TabBar_0/n=tabGridView_0'); // Button text: 'GridView'
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyViewPage_0/c=TabBar_0/n=tabListView_0'); // Button text: 'ListView'
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyViewPage_0/c=TabBar_0/n=tabPathView_0'); // Button text: 'PathView'
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyViewPage_0/c=TabBar_0/n=tabSwipeView_0'); // Button text: 'SwipeView'
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyViewPage_0/c=TabBar_0/n=tabTableView_0'); // Button text: 'TableView'
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickTabBar_0/n=textItemsTabButton_0');
    // Looks like focus click on TextInput
    // QtAda.mouseClick('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_0/n=simpleTextInput_0', 'LeftButton', 126, 15);
    QtAda.setText('c=ApplicationWindow_0/c=QQuickStackLayout_0/c=MyTextPage_0/c=QQuickColumnLayout_0/c=QQuickRowLayout_0/c=QQuickRectangle_0/n=simpleTextInput_0', 'Original TextIn7put');
    QtAda.buttonClick('c=ApplicationWindow_0/c=QQuickTabBar_0/n=clickItemsTabButton_0');
}
test();
