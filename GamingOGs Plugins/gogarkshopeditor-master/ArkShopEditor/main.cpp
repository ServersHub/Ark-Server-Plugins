#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

extern "C" void __chkstk();

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <codecvt>
#include <limits>
#include <array>
#include <fstream>
#include <QCryptographicHash>

#include "cppcodec/base64_rfc4648.hpp"

#include "mainwindow.h"
namespace snowapril {
    constexpr int RandomSeed(void) {
        return '0'      * -40271 + // offset accounting for digits' ANSI offsets
            __TIME__[7] *      1 +
            __TIME__[6] *     10 +
            __TIME__[4] *     60 +
            __TIME__[3] *    600 +
            __TIME__[1] *   3600 +
            __TIME__[0] *  36000;
    };

    template <unsigned int a,
              unsigned int c,
              unsigned int seed,
              unsigned int Limit>
    struct LinearCongruentialEngine {
        enum { value = (a * LinearCongruentialEngine<a, c - 1, seed, Limit>::value + c) % Limit };
    };

    template <unsigned int a,
              unsigned int seed,
              unsigned int Limit>
    struct LinearCongruentialEngine<a, 0, seed, Limit> {
        enum { value = (a * seed) % Limit };
    };

    template <int N, int Limit>
    struct MetaRandom {
        enum { value = LinearCongruentialEngine<16807, N, RandomSeed(), Limit>::value };
    };

    template <int A, int B>
        struct ExtendedEuclidian { enum {
            d = ExtendedEuclidian<B, A % B>::d,
            x = ExtendedEuclidian<B, A % B>::y,
            y = ExtendedEuclidian<B, A % B>::x - (A/B) * ExtendedEuclidian<B, A % B>::y
        }; };

        template <int A>
        struct ExtendedEuclidian<A, 0> { enum {
            d = A,
            x = 1,
            y = 0
        }; };

        constexpr std::array<int, 30> PrimeNumbers = {
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
            31, 37, 41, 43, 47, 53, 59, 61, 67,
            71, 73, 79, 83, 89, 97, 101, 103,
            107, 109, 113
        };

        constexpr int positive_modulo(int a, int n) {
            return (a % n + n) % n;
        }

        template <typename Indexes, int A, int B>
        class MetaString;

        template <size_t... I, int A, int B>
        class MetaString<std::index_sequence<I...>, A, B> {
        public:
            constexpr MetaString(char const* str)
                : encrypted_buffer{ encrypt(str[I])... } {};
        public:
            inline const char* decrypt(void) {
                for (size_t i = 0; i < sizeof...(I); ++i) {
                    buffer[i] = decrypt(encrypted_buffer[i]);
                }
                buffer[sizeof...(I)] = 0;
                return buffer;
            }
        private:
            constexpr int  encrypt(char c) const { return (A * c + B) % 127; } ;
            constexpr char decrypt(int c) const { return positive_modulo(ExtendedEuclidian<127, A>::y * (c - B), 127); } ;
        private:
            char buffer[sizeof...(I) + 1] {};
            int  encrypted_buffer[sizeof...(I)] {};
        };

}

#define TEST(str) (snowapril::MetaString<std::make_index_sequence<sizeof(str) - 1>, \
                          std::get<snowapril::MetaRandom<__COUNTER__, 30>::value>(snowapril::PrimeNumbers), \
                          snowapril::MetaRandom<__COUNTER__, 126>::value>(str))

#define OBFUSCATE(str) (snowapril::MetaString<std::make_index_sequence<sizeof(str) - 1>, \
                          std::get<snowapril::MetaRandom<__COUNTER__, 30>::value>(snowapril::PrimeNumbers), \
                          snowapril::MetaRandom<__COUNTER__, 126>::value>(str).decrypt())

QByteArray GetHWID()
{
    auto tchar_to_string = [](const TCHAR* input) -> std::string
    {
        const std::wstring wide_hwid = static_cast<const wchar_t*>(input);
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wide_hwid);
    };

    TCHAR volumeName[MAX_PATH + 1] = { 0 };
    TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;
    GetVolumeInformation(L"C:\\", volumeName, ARRAYSIZE(volumeName), &serialNumber, &maxComponentLen,
        &fileSystemFlags, fileSystemName, ARRAYSIZE(fileSystemName));

    TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName) / sizeof(computerName[0]);
    GetComputerName(computerName, &size);

    std::string decrypted{};
    decrypted.append(tchar_to_string(volumeName));
    decrypted.append(tchar_to_string(fileSystemName));
    decrypted.append(std::to_string(serialNumber));
    decrypted.append(std::to_string(maxComponentLen));
    decrypted.append(std::to_string(fileSystemFlags));
    decrypted.append(tchar_to_string(computerName));

    QByteArray ret = QCryptographicHash::hash(decrypted.c_str(), QCryptographicHash::Md5);
    return ret.toHex();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    try
    {
        QFile f(QCoreApplication::applicationDirPath() + "/license.txt");
        if(!f.exists())
        {
            QMessageBox box;
            box.setText("License file is not found. Please put file with your license key together with ArkShopEditor.exe");
            box.exec();
            return 0;
        }
        if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QMessageBox box;
            box.setText("Can't open license file");
            box.exec();
            return 0;
        }
        QTextStream s(&f);
        std::string licenseKey = s.readLine().toStdString();

        httplib::Client client(OBFUSCATE("https://dashboard.gameservershub.com"));
        const std::string body = "{\"licensekey\":\"" + licenseKey +
            "\",\"product\":\"" + OBFUSCATE("ArkShopEditor") +
            "\",\"version\":\"" + OBFUSCATE("1.0") +
            "\",\"hwid\":\"" + GetHWID().data() + "\"}";

        auto response = client.Post("/api/client",
            httplib::Headers{ {"Authorization", OBFUSCATE("de2dab4100282eddec3193617c92c29765a0c075")} },
            body,
            "application/json");

        if (!response)
        {
            QMessageBox box;
            box.setText("Authentication failed: request error");
            box.exec();
            return 0;
        }

        if (response->status == 200 && response->body.find("\"status_overview\":\"success\"") != std::string::npos)
        {
            const auto body = response->body;
            const auto idBegin = body.find("status_id") + 12;
            const auto id = body.substr(idBegin, 24);
            const auto encoded = id.substr(0, id.find("694201337"));
            const auto decodedTmp = cppcodec::base64_rfc4648::decode(encoded);
            const auto decoded = std::string{ decodedTmp.begin(), decodedTmp.end() };
            const auto licenseFirst = licenseKey.substr(0, 2);
            const auto licenseLast = licenseKey.substr(licenseKey.size() - 2, 2);
            const auto apiKeyFirst = std::string{ OBFUSCATE("de2dab4100282eddec3193617c92c29765a0c075") }.substr(0, 2);
            const auto code = licenseFirst + licenseLast + apiKeyFirst;

            if (decoded == code)
            {
                QFile f(":qdarkstyle/style.qss");

                if (!f.exists())   {
                    printf("Unable to set stylesheet, file not found\n");
                }
                else   {
                    f.open(QFile::ReadOnly | QFile::Text);
                    QTextStream ts(&f);
                    app.setStyleSheet(ts.readAll());
                }

                MainWindow window;
                window.show();

                return app.exec();
            }
            else
            {
                QMessageBox box;
                box.setText("Authentication failed: wrong code");
                box.exec();
                return 0;
            }
        }
        else
        {
            QMessageBox box;
            box.setText(QString("Authentication failed: %1").arg(nlohmann::json::parse(response->body)["status_msg"].get<std::string>().c_str()));
            box.exec();
            return 0;

        }
    }
    catch (const std::exception& e)
    {
        QMessageBox box;
        box.setText(QString("Exception: %1").arg(e.what()));
        box.exec();
        return 0;

    }
}
