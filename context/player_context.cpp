#include "player_context.hpp"

#include <QCoreApplication>

PlayerContext* PlayerContext::s_instance = nullptr;

PlayerContext* PlayerContext::instance() {
    if (!s_instance) {
        s_instance = new PlayerContext(QCoreApplication::instance());
    }
    return s_instance;
}

PlayerContext::PlayerContext(QObject* parent)
    : QObject(parent), m_controller(nullptr), m_library(nullptr), m_queue(nullptr) {}

void PlayerContext::initialize(Controller* controller, Library* library, Queue* queue) {
    m_controller = controller;
    m_library = library;
    m_queue = queue;
}


