#include "bitmap.h"
#include <QtTest>


class testBMP : public QObject
{
    Q_OBJECT

public:
    testBMP();
    ~testBMP();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testLoad();
    void testInvalidLoad1();
    void testInvalidLoad2();
    void testSave();

    // more tests need to be added

private:
    BitMap* m_image;

};

testBMP::testBMP()
{
    m_image = nullptr;
}

testBMP::~testBMP()
{
    m_image = nullptr;
}

void testBMP::initTestCase()
{

}

void testBMP::cleanupTestCase()
{
    if(m_image)
        delete m_image;
}



void testBMP::testLoad()
{
    std::string filename = "resources/8_bit.bmp";
    m_image = new BitMap(filename);

    QVERIFY(m_image->height() > 0);
    QVERIFY(m_image->width() > 0);
    QVERIFY(m_image->pixel_data().size() > 0);
}

void testBMP::testInvalidLoad1()
{
    std::string filename = "resources/24_bit.bmp";

    bool exception = false;
    try
    {
        m_image = new BitMap(filename);
    }
    catch(...)
    {
        exception = true;
    }

    QVERIFY(exception == true);

}

void testBMP::testInvalidLoad2()
{
    std::string filename = "resources/unkown.bmp";
    bool exception = false;
    try
    {
        m_image = new BitMap(filename);
    }
    catch(...)
    {
        exception = true;
    }

    QVERIFY(exception == true);

}

void testBMP::testSave()
{
    std::string filename = "resources/8_bit.bmp";
    std::string saved_filename = "resources/8_bit_saved.bmp";

    bool exception = false;
    try
    {
        m_image = new BitMap(filename);
        m_image->save(saved_filename);

    }
    catch(...)
    {
        exception = true;
    }

    QVERIFY(exception == false);

}

QTEST_APPLESS_MAIN(testBMP)

#include "tst_testbmp.moc"
