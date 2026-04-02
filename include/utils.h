#pragma once
#include <map>


enum class ActionId {
    EXPORT,
    GET_TEMPLATE,
    GET_ALL_TEMPLATES,
    ADD_TEMPLATE,
    REMOVE_TEMPLATE,
    ADD_TAG,
    REMOVE_TAG
};

const std::map<ActionId, const QString> ACTIONS_MAP = {
    {ActionId::EXPORT, "Экспорт Шаблона"},
    {ActionId::GET_TEMPLATE, "Показать шаблон"},
    {ActionId::GET_ALL_TEMPLATES, "Все шаблоны"},
    {ActionId::ADD_TEMPLATE, "Добавить/изменить шаблон"},
    {ActionId::REMOVE_TEMPLATE, "Удалить шаблон"},
    {ActionId::ADD_TAG, "Добавить/изменить тэг"},
    {ActionId::REMOVE_TAG, "Удалить тэг"},
};