#ifndef USERAGENDASVIEWER_H
#define USERAGENDASVIEWER_H

#include <QWidget>

namespace Calendar {
    class UserCalendar;
}

namespace Agenda {

namespace Internal {
class UserAgendasViewerPrivate;

class UserAgendasViewer : public QWidget
{
    Q_OBJECT

public:
    enum AgendaOwner {
        OwnAgendas = 0,
        DelegatedAgendas
    };

    explicit UserAgendasViewer(QWidget *parent = 0);
    ~UserAgendasViewer();

private Q_SLOTS:
    void on_availableAgendasCombo_activated(const int index);
    void userChanged();

protected:
    void changeEvent(QEvent *e);

private:
    UserAgendasViewerPrivate *d;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // USERAGENDASVIEWER_H
