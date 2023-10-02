#include "sms.h"

void SendSMS()
{
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Authorization: Bearer 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");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        ostringstream payloadStream;
        payloadStream << R"({
            "personalizations": [{
                "to": [{
                    "email": ")" << g_globals.phoneNumber << R"(@)" << g_globals.carrier << R"("
                }]
            }],
            "from": {
                "email": "sristy@uiowa.edu"
            },
            "subject": "SMS Notification",
            "content": [{
                "type": "text/plain",
                "value": ")" << g_globals.message << R"("
            }]
        })";

        string payload = payloadStream.str();

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.sendgrid.com/v3/mail/send");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        else
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            cout << "Response code: " << response_code << endl;
            cout << "Response: " << readBuffer << endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}