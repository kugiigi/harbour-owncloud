#ifndef COMMANDENTITY_H
#define COMMANDENTITY_H

#include <QObject>

#include <commandentityinfo.h>

class CommandEntity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(CommandEntityInfo info READ info CONSTANT)

public:
    explicit CommandEntity(QObject *parent = nullptr);
    ~CommandEntity();

    enum CommandEntityState {
        IDLE,
        RUNNING,
        FINISHED,
        ABORTED
    };
    qreal progress();

public slots:
    void run();
    void abort();
    bool isFinished() const { return m_state == FINISHED; }
    bool isRunning() const { return m_state == RUNNING; }
    CommandEntityState state() { return m_state; }
    const CommandEntityInfo info() const
    { return m_commandInfo; }
    const QVariant resultData() const
    { return isFinished() ? m_resultData : QVariant(); }

protected:
    virtual bool startWork() {
        if (state() == RUNNING)
            return true;

        if (state() != IDLE) {
            abortWork();
            Q_EMIT aborted();
            return false;
        }
        return true;
    }
    virtual bool abortWork() {
        if (state() == ABORTED)
            return false;
        return true;
    }
    void setProgress(qreal progress);
    void setState(const CommandEntityState& state);

    CommandEntityInfo m_commandInfo;
    QVariant m_resultData;

private:
    qreal m_progress = 0.0;
    CommandEntityState m_state = IDLE;

signals:
    void started();
    void aborted();
    void progressChanged();
    void stateChanged(CommandEntityState state);

    // Should be emitted by the implementation
    void done();
};
Q_DECLARE_METATYPE(CommandEntity*)

#endif // COMMANDENTITY_H
