#include "objectmanager.h"

#include "resource_management/fontmanager.h"

#include "coreengine/audiothread.h"
#include "coreengine/mainapp.h"
#include "objects/label.h"

ObjectManager::ObjectManager()
    : RessourceManagement<ObjectManager>("/objects/res.xml", "")
{
    loadRessources("/cursor/res.xml");
}

oxygine::spButton ObjectManager::createButton(QString text, qint32 width)
{
    oxygine::spButton pButton = new oxygine::Button();
    pButton->setResAnim(ObjectManager::getInstance()->getResAnim("button"));
    pButton->setPriority(static_cast<short>(Mainapp::ZOrder::Objects));

    //Create Actor with Text and add it to button as child
    spLabel textField = new Label(width - 10);
    oxygine::TextStyle style = FontManager::getMainFont24();
    style.color = FontManager::getFontColor();
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;
    textField->setStyle(style);
    textField->setHtmlText(text);
    if (width < 0)
    {
        width = textField->getTextRect().getWidth();
        if (width < 180)
        {
            width = 180;
        }
    }
    textField->setWidth(width - 10);
    textField->setPosition(width / 2 - textField->getTextRect().getWidth() / 2, 5);
    if (textField->getX() < 5)
    {
        textField->setX(5);
    }
    pButton->setSize(width, 40);
    oxygine::spClipRectActor clipRect = new oxygine::ClipRectActor();
    clipRect->setSize(pButton->getSize());
    textField->setSize(pButton->getSize());
    textField->attachTo(clipRect);
    clipRect->attachTo(pButton);

    oxygine::Sprite* ptr = pButton.get();
    pButton->addEventListener(oxygine::TouchEvent::OVER, [ = ](oxygine::Event*)
    {
        ptr->addTween(oxygine::Sprite::TweenAddColor(QColor(16, 16, 16, 0)), oxygine::timeMS(300));
    });

    pButton->addEventListener(oxygine::TouchEvent::OUTX, [ = ](oxygine::Event*)
    {
        ptr->addTween(oxygine::Sprite::TweenAddColor(QColor(0, 0, 0, 0)), oxygine::timeMS(300));
    });
    pButton->addEventListener(oxygine::TouchEvent::CLICK, [ = ](oxygine::Event*)
    {
        Mainapp::getInstance()->getAudioThread()->playSound("button.wav");
    });
    return pButton;
}


oxygine::spButton ObjectManager::createIconButton(QString icon)
{
    oxygine::spButton pButton = new oxygine::Button();
    pButton->setResAnim(ObjectManager::getInstance()->getResAnim("button_square"));
    pButton->setPriority(static_cast<short>(Mainapp::ZOrder::Objects));
    pButton->setSize(30, 30);

    oxygine::spSprite pSprite = new oxygine::Sprite();
    pSprite->setResAnim(ObjectManager::getInstance()->getResAnim(icon));
    pButton->addChild(pSprite);

    oxygine::Sprite* ptr = pButton.get();
    pButton->addEventListener(oxygine::TouchEvent::OVER, [ = ](oxygine::Event*)
    {
        ptr->addTween(oxygine::Sprite::TweenAddColor(QColor(16, 16, 16, 0)), oxygine::timeMS(300));
    });

    pButton->addEventListener(oxygine::TouchEvent::OUTX, [ = ](oxygine::Event*)
    {
        ptr->addTween(oxygine::Sprite::TweenAddColor(QColor(0, 0, 0, 0)), oxygine::timeMS(300));
    });
    pButton->addEventListener(oxygine::TouchEvent::CLICK, [ = ](oxygine::Event*)
    {
        Mainapp::getInstance()->getAudioThread()->playSound("button.wav");
    });
    return pButton;
}
