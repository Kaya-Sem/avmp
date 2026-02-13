#ifndef PLAYER_CONTEXT
#define PLAYER_CONTEXT

#include "library/library.hpp"
#include "library/playlist_manager.hpp"
#include "controller/controller.hpp"
#include "queue/queue.hpp"
#include "qtmetamacros.h"
#include <QObject>

class PlayerContext : public QObject {
    Q_OBJECT

    Q_PROPERTY(Controller* controller READ controller CONSTANT)
    Q_PROPERTY(Library* library READ library CONSTANT)
    Q_PROPERTY(Queue* queue READ queue CONSTANT)
    Q_PROPERTY(PlaylistManager* playlistManager READ playlistManager CONSTANT)

public:
    static PlayerContext* instance();

    Controller* controller() const { return m_controller; }
    Library* library() const { return m_library; }
    Queue* queue() const { return m_queue; }
    PlaylistManager* playlistManager() const { return m_playlistManager; }

    void initialize(Controller* controller, Library* library, Queue* queue);

private:
    static PlayerContext* s_instance;
    explicit PlayerContext(QObject* parent = nullptr);

    Controller* m_controller;
    Library* m_library;
    Queue* m_queue;
    PlaylistManager* m_playlistManager;
};

#endif // !PLAYER_CONTEXT
