#pragma once
#include "../../src/glpch.h"
#include <unordered_map>
#include <vector>
#include <functional>

#include <iostream>

class InputManager {
public:
    static InputManager& Instance();

    void Update();

    bool IsKeyPressed(int key) const;
    bool IsKeyJustPressed(int key) const;
    bool IsMouseButtonDown(int button) const;
    bool IsMouseButtonJustPressed(int button) const;
    bool IsMouseButtonJustReleased(int button) const;
    void EndFrame();

    //// Métodos para configurar el input real (por ejemplo, desde tu clase Application).
    void SetKeyPressed(int key, bool isPressed);
    void SetMousePressed(int button, double x, double y, bool isPressed);

    void SetMousePosition(double x, double y);
    void SetViewportSize(int width, int height);
    std::pair<double, double> GetMousePosition() const;
    std::pair<double, double> GetMouseDelta() const;

    void subscribe()
    {
        // Suscribir una función al evento de redimensionamiento de la ventana
        EventManager::getWindowResizeEvent().subscribe([this](int width, int height)
            {
                m_viewportSize = { width, height };
        });
    }

private:
    InputManager() {}

    std::unordered_map<int, std::vector<std::function<void()>>> m_keySubscribers;
    std::unordered_map<int, bool> m_lastKeyStates;

    std::unordered_map<int, std::vector<std::function<void(double, double)>>> m_mouseSubscribers;
    std::unordered_map<int, std::pair<double, double>> m_lastMouseStates;

    std::unordered_map<int, bool> m_keyStates;
    std::unordered_map<int, std::pair<double, double>> m_mouseStates;

    std::pair<double, double> m_mousePosition = { 0.0, 0.0 };
    std::pair<double, double> m_lastMousePosition = { 0.0, 0.0 };
    std::pair<int, int> m_viewportSize = { 800, 600 };  // Asumiendo un tamaño inicial de 800x600
};
