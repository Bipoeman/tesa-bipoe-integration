#include "../main.h"

void *file_upload_thread(void *) {
    while (1) {
        printf("Waiting for file name to upload\n");
        pthread_cond_wait(&http_cond, &http_cond_mutex);
        printf("Uploading...\n");
        upload_file_to_http(http_server_url, httpFilePath, bearerToken,httpTimeStamp,serialNumber);
        printf("Uploaded\n");
    }
}

int upload_file_to_http(const char *url, const char *file_path, const char *bearer_token, const char *timestamp, const char *device_id) {
    CURL *curl;
    CURLcode res;
    FILE *file;

    // Open the file to upload
    file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the HTTP POST request
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Specify that we're doing a POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Initialize the mime structure for the POST form
        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);

        // Add the file field to the form
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");           // The field name for the file
        curl_mime_filedata(part, file_path);    // The file to upload

        // Add timestamp as a separate form field
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "timeStamp");
        curl_mime_data(part, timestamp, CURL_ZERO_TERMINATED);

        // Add deviceId as a separate form field
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "deviceId");
        curl_mime_data(part, device_id, CURL_ZERO_TERMINATED);

        // Attach the mime form to the request
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Set up Bearer Token authentication
        struct curl_slist *headers = NULL;
        char auth_header[1024];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", bearer_token);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the file upload
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("File uploaded successfully.\n");
        }

        // Clean up
        curl_mime_free(mime);                  // Free the mime form
        curl_easy_cleanup(curl);               // Cleanup curl session
        curl_slist_free_all(headers);          // Free headers list
    } else {
        fprintf(stderr, "Failed to initialize libcurl.\n");
    }

    // Close the file
    fclose(file);

    return res;
}