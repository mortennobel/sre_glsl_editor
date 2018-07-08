//
// Created by Morten Nobel-Jørgensen on 7/6/18.
//

#pragma once

class GLSLEditor;

class UniformComponent {
public:
    explicit UniformComponent(GLSLEditor* editor);
    void gui();
private:
    GLSLEditor* editor;
};


