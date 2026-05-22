#pragma once

#include <QString>
#include <map>


enum class ActionId {
    EXPORT,
    GET_TEMPLATE,
    GET_ALL_TEMPLATES,
    ADD_TEMPLATE,
    DELETE_TEMPLATE,
    GET_ALL_TAGS,
    GET_TAG,
    NONE_ACTION
};

// BramSpace Print host
const QString BASE_URL = "http://localhost:8000";

//Buttons Labels
const QString PrintButtonLabel = "BramSpacePrint";

const QString OkButtonLabel = "Ок";
const QString CancelButtonLabel = "Отмена";

const QString NameLabel = "Имя";
const QString DescriptionLabel = "Описание";
const QString SubsystemLabel = "Подсистема";
const QString AliasLabel = "Псевдоним в JSON";

const QString TemplateIdLabel = "Id шаблона";
const QString ElementIdLabel = "Id элемента";

const QString PrintTitle = "Печать";
const QString ErrorTitle = "Ошибка";
