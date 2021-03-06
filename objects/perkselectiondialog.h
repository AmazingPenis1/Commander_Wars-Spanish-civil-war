#ifndef PERKSELECTIONDIALOG_H
#define PERKSELECTIONDIALOG_H

#include <QObject>
#include "game/player.h"

#include "objects/panel.h"
#include "objects/perkselection.h"
#include "objects/dropdownmenu.h"

#include "oxygine-framework.h"

class PerkSelectionDialog;
typedef oxygine::intrusive_ptr<PerkSelectionDialog> spPerkSelectionDialog;

class PerkSelectionDialog : public QObject, public oxygine::Actor
{
    Q_OBJECT
public:
    explicit PerkSelectionDialog(Player* pPlayer, qint32 maxPerkcount, bool banning);
protected slots:
    void changeCO(qint32 index);
    /**
     * @brief showSaveBannlist
     */
    void showSavePerklist();
    /**
     * @brief saveBannlist
     */
    void savePerklist(QString filename);
    /**
     * @brief savePerklist
     * @param item
     */
    void setPerkBannlist(qint32 item);
signals:
    void sigCancel();
    void sigFinished();
    void editFinished(QStringList list);
    void sigToggleAll(bool toggle);
    void sigShowSavePerklist();
private:
    oxygine::spButton m_OkButton;
    oxygine::spButton m_CancelButton;
    oxygine::spButton m_ToggleAll;
    spDropDownmenu m_PredefinedLists;
    bool toggle{true};
    Player* m_pPlayer{nullptr};
    spPanel m_pPanel;
    spPerkSelection m_pPerkSelection;
};

#endif // PERKSELECTIONDIALOG_H
