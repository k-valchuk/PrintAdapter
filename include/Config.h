#pragma once

#include <QString>
#include <map>


enum class ActionId {
    EXPORT,
    GET_TEMPLATE,
    GET_ALL_TEMPLATES,
    ADD_TEMPLATE,
    REMOVE_TEMPLATE,
    ADD_TAG,
    REMOVE_TAG,
    GET_ALL_TAGS,
};

const std::map<ActionId, const QString> ACTIONS_MAP = {
    {ActionId::EXPORT, "Экспорт Шаблона"},
    {ActionId::GET_TEMPLATE, "Показать шаблон"},
    {ActionId::GET_ALL_TEMPLATES, "Все шаблоны"},
    {ActionId::ADD_TEMPLATE, "Добавить/изменить шаблон"},
    {ActionId::REMOVE_TEMPLATE, "Удалить шаблон"},
    {ActionId::GET_ALL_TAGS, "Все тэги"},
    {ActionId::ADD_TAG, "Добавить/изменить тэг"},
    {ActionId::REMOVE_TAG, "Удалить тэг"},
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