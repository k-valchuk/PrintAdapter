#ifndef THUMBSTRUCTS_H
#define THUMBSTRUCTS_H
//-------------------------------------------------------------------------------------------------
// Данные для создания миниатюр
//-------------------------------------------------------------------------------------------------

#include <malloc.h>
#include <string>

// Миниатюра
struct ThumbItem
{
    virtual ~ThumbItem(){};

    int id = -1;                // Идентификатор
    std::wstring filePath;      // Файл
};

// Данные изображения миниатюры
struct ThumbImageData
{
    virtual ~ThumbImageData()
    {// Освобождаем выделенную под изображение память
        if (data)
        {
#ifdef WIN32
            _aligned_free(data);
#else
            free(data);
#endif
            data = nullptr;
        }
    }

    char *data = nullptr;   // Данные изображения
    int width = 0;          // Ширина изображения
    int height = 0;         // Высота изображения
};
#endif // THUMBSTRUCTS_H
