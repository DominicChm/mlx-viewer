#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MLX90640.h>
#define IMG_WIDTH 32
#define IMG_HEIGHT 24
#define IMG_PIXELS IMG_HEIGHT *IMG_WIDTH
#define ADDR 0x33

/**
 * Viewer protocol:
 * Total packet format is <0xFE><CMD><LEN><LEN><DATA>
 */

struct CamInfo
{
    bool en_subpage_mode;
    bool en_data_hold;
    bool en_subpage_repeat;
    uint8_t select_subpage;
    uint8_t refresh_rate;
    uint8_t resolution;
    uint8_t pattern;
} __attribute__((packed));

class MLX
{
public:
    const short ADDR_STATUS = 0x8000;
    const short ADDR_CR1 = 0x800D;

    float temp_frame[IMG_PIXELS]; // buffer for full frame of temperatures
    uint16_t raw_frame[834];

    Adafruit_MLX90640 amlx;
    float ta = 0;
    TwoWire &w;

    struct
    {
        long t_frame_fetch;
        long t_frame_tx_time;
        long t_calc_time;
    } cam_timing;

    struct
    {
        float cx;
        float cy;
    } analysis;
    union
    {
        struct
        {
            unsigned en_subpage_mode : 1;
            unsigned : 1;
            unsigned en_data_hold : 1;
            unsigned en_subpage_repeat : 1;
            unsigned select_subpage : 3;
            unsigned refresh_rate : 3;
            unsigned resolution : 2;
            unsigned use_chessboard_pattern : 1;
        } cr1;
        uint16_t cr1_word;
    };

    MLX(TwoWire &wire) : w(wire)
    {
    }

    void write_cam_params()
    {
        amlx.MLX90640_I2CWrite(ADDR, ADDR_CR1, cr1_word);
    }

    void read_cam_params()
    {
        amlx.MLX90640_I2CRead(ADDR, ADDR_CR1, 1, &cr1_word);
    }

    bool init()
    {
        delay(80);
        tx_debug("Initializing MLX90640!");
        int status = 0;

        amlx.begin(ADDR, &Wire);
        cr1.en_subpage_mode = 1;
        cr1.en_data_hold = 0;
        cr1.en_subpage_repeat = 0;
        cr1.refresh_rate = 5;
        cr1.resolution = 2;
        cr1.use_chessboard_pattern = 0;

        write_cam_params();
        read_cam_params();

        tx_debug("Initialized!");
        return true;
    }

    bool has_img()
    {
        return false;
    }

    long read_img()
    {

        float emissivity = 0.95;
        int status;

        long t = millis();
        status = amlx.MLX90640_GetFrameData(0, raw_frame);
        cam_timing.t_frame_fetch = millis() - t;

        if (status < 0)
        {
            return status;
        }

        ta = amlx.MLX90640_GetTa(raw_frame, &amlx._params) - OPENAIR_TA_SHIFT; // For a MLX90640 in the open air the shift is -8

        t = millis();
        amlx.MLX90640_CalculateTo(raw_frame, &amlx._params, emissivity, ta, temp_frame);
        cam_timing.t_calc_time = millis() - t;

        return 0;
    }

    void tx_current_image()
    {
        long t = millis();
        tx(0x00, &temp_frame);
        cam_timing.t_frame_tx_time = millis() - t;
    }

    void tx_debug(const char *msg)
    {
        tx(0x01, msg, strlen(msg));
    }

    void tx_debugf(const char *fmt, ...)
    {
        char msg[255];

        va_list args;
        va_start(args, fmt);
        tx(0x01, msg, vsnprintf(msg, 255, fmt, args));
        va_end(args);
    }

    void tx_current_params()
    {
        // CamInfo c{
        //     cr1.en_subpage_mode,
        //     cr1.en_data_hold,
        //     cr1.en_subpage_repeat,
        //     static_cast<uint8_t>(cr1.select_subpage),
        //     static_cast<uint8_t>(cr1.refresh_rate),
        //     static_cast<uint8_t>(cr1.resolution),
        //     static_cast<uint8_t>(cr1.use_chessboard_pattern),
        // };

        // tx(0x02, &c);
    }

    void detect_mutating()
    {
        float temp_min = ta + 4;
        float temp_max = 38;

        float amax = -100.;

        for (size_t i; i < IMG_PIXELS; i++)
        {
            if (temp_frame[i] > temp_max || temp_frame[i] < temp_min)
            {
                temp_frame[i] = temp_min;
            }

            amax = max(temp_frame[i], amax);
        }

        for (size_t i; i < IMG_PIXELS; i++)
        {
        }
    }

    void tx_timings()
    {
        tx(0x03, &cam_timing);
    }

    template <typename T>
    void tx(uint8_t cmd, T *t, uint16_t len = sizeof(T))
    {
        Serial.write(0xA0);
        Serial.write(cmd); // CMD
        Serial.write((uint8_t *)&len, 2);
        Serial.write((uint8_t *)t, len);
    }
};
