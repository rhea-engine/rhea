#include <gtest/gtest.h>
#include <Rhea/Application.h>
#include <Rhea/EventInput.h>
#include <Rhea/GUI/TextInput.h>

std::vector<std::unique_ptr<rhea::IFabric>> createFabrics() { return {}; }

namespace
{
    std::string registerInputType()
    {
        simpleparser::Type::registerType<rhea::TextInput>("//meworla/gui/textinput");
        return "input";
    }

    // Exercise editing without a window, renderer or platform clipboard.
    class TestTextInput : public rhea::TextInput
    {
    public:
        TestTextInput() : TextInput(registerInputType()) { m_has_focus = true; }
        using TextInput::moveCursor;
        using TextInput::replaceSelection;
        using TextInput::selectWord;

        size_t cursor() const { return m_pos; }
        void key(rhea::Key key, bool shift = false, bool ctrl = false)
        {
            rhea::InputEvent input;
            input.type = rhea::InputEventType::KeyPressed;
            input.key = key;
            input.modifiers.shift = shift;
            input.modifiers.ctrl = ctrl;
            std::vector<std::string> actions;
            rhea::EventInput event("input", input, actions);
            onKeyPressed(&event, nullptr);
        }
        void type(const std::string& text)
        {
            rhea::InputEvent input;
            input.type = rhea::InputEventType::Text;
            input.text = text;
            std::vector<std::string> actions;
            rhea::EventInput event("input", input, actions);
            onTextInput(&event, nullptr);
        }
    };

    TEST(TextInputSelection, ShiftArrowsExtendAndReverseAcrossAnchor)
    {
        TestTextInput input;
        input.setText("abcdef");
        input.moveCursor(3, false);
        input.key(rhea::Key::Left, true);
        input.key(rhea::Key::Left, true);
        EXPECT_EQ(input.getSelectedText(), "bc");
        input.key(rhea::Key::Right, true);
        input.key(rhea::Key::Right, true);
        EXPECT_FALSE(input.hasSelection());
        input.key(rhea::Key::Right, true);
        EXPECT_EQ(input.getSelectedText(), "d");
    }

    TEST(TextInputSelection, PlainArrowsCollapseToRespectiveEdge)
    {
        TestTextInput input;
        input.setText("abcdef");
        input.moveCursor(2, false);
        input.moveCursor(5, true);
        input.key(rhea::Key::Left);
        EXPECT_EQ(input.cursor(), 2);
        EXPECT_FALSE(input.hasSelection());
        input.moveCursor(5, true);
        input.key(rhea::Key::Right);
        EXPECT_EQ(input.cursor(), 5);
        EXPECT_FALSE(input.hasSelection());
    }

    TEST(TextInputSelection, TypingAndPasteReplaceForwardAndBackwardRanges)
    {
        TestTextInput input;
        input.setText("abcdef");
        input.moveCursor(1, false);
        input.moveCursor(4, true);
        input.type("XY");
        EXPECT_EQ(input.getText(), "aXYef");
        EXPECT_EQ(input.cursor(), 3);
        EXPECT_FALSE(input.hasSelection());
        input.moveCursor(1, true);
        input.replaceSelection("paste");
        EXPECT_EQ(input.getText(), "apasteef");
        EXPECT_FALSE(input.hasSelection());
    }

    TEST(TextInputSelection, DeleteAndBackspaceRemoveSelectionAndHandleBoundaries)
    {
        for (auto key : {rhea::Key::DeleteKey, rhea::Key::Backspace}) {
            TestTextInput input;
            input.setText("abcdef");
            input.moveCursor(4, false);
            input.moveCursor(1, true);
            input.key(key);
            EXPECT_EQ(input.getText(), "aef");
            EXPECT_EQ(input.cursor(), 1);
            EXPECT_FALSE(input.hasSelection());
            input.moveCursor(0, false);
            input.key(rhea::Key::Backspace);
            input.key(rhea::Key::End);
            input.key(rhea::Key::DeleteKey);
            EXPECT_EQ(input.getText(), "aef");
        }
    }

    TEST(TextInputSelection, SelectAllHomeEndAndProgrammaticReset)
    {
        TestTextInput input;
        input.setText("abcdef");
        input.key(rhea::Key::A, false, true);
        EXPECT_EQ(input.getSelectedText(), "abcdef");
        input.type("new");
        EXPECT_EQ(input.getText(), "new");
        input.key(rhea::Key::Home, true);
        EXPECT_EQ(input.getSelectedText(), "new");
        input.setText("");
        EXPECT_FALSE(input.hasSelection());
        input.key(rhea::Key::Left, true);
        input.key(rhea::Key::Right, true);
        EXPECT_EQ(input.cursor(), 0);
    }

    TEST(TextInputSelection, DoubleClickSelectsPathSegmentsIncludingExtension)
    {
        TestTextInput input;
        input.setText("file:///C:/Worlds/my-scene.scene");
        input.selectWord(23);
        EXPECT_EQ(input.getSelectedText(), "my-scene.scene");
        input.selectWord(13);
        EXPECT_EQ(input.getSelectedText(), "Worlds");
        input.setText("hello world");
        input.selectWord(8);
        EXPECT_EQ(input.getSelectedText(), "world");
    }

    TEST(TextInputSelection, ControlTextDoesNotDeleteSelection)
    {
        TestTextInput input;
        input.setText("keep");
        input.selectAll();
        input.type("\b\r\n\t");
        EXPECT_EQ(input.getText(), "keep");
        EXPECT_EQ(input.getSelectedText(), "keep");
    }
}
