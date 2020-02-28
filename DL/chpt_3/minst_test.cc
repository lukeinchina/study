#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* 字节序转换用到 */
#include <arpa/inet.h>

#include "network.h"
#include "types.h"

#define MAGIC_LABEL 0x00000801
#define MAGIC_IMAGE 0x00000803

enum {
    INPUT_LAYER_COUNT = 784,
    HIDDEN_LAYER_COUNT = 300,
    OUTPUT_LAYER_COUNT = 10
};

/*  function deceration */
int load_minst_image(const char *path, std::vector<Sample *> &samples);
int load_minst_label(const char *path, std::vector<Label *> &labels);

double evaluate(Network *net, 
        const std::vector<Sample *> &test_samples,
        const std::vector<Label *>  &test_labels);

void usage(const char *prog_name) {
    printf("%s\t rate round \n", prog_name);
    printf("\tdefault input layer: %d nodes\n", INPUT_LAYER_COUNT);
    printf("\thidden layer:%d nodes\n", HIDDEN_LAYER_COUNT);
    printf("\toutput layer:%d nodes\n", OUTPUT_LAYER_COUNT);
    return;
}

int
main(int argc, char *argv[]) {
    const char *train_image_path = "/Users/a58/code/opendata/minst/train-images-idx3-ubyte";
    const char *train_label_path = "/Users/a58/code/opendata/minst/train-labels-idx1-ubyte";
    const char *test_image_path = "/Users/a58/code/opendata/minst/t10k-images-idx3-ubyte";
    const char *test_label_path = "/Users/a58/code/opendata/minst/t10k-labels-idx1-ubyte";

    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }
    double rate = atof(argv[1]);
    int round = atoi(argv[2]);
    srandom(time(NULL));

    std::vector<int> node_counts;
    node_counts.push_back(INPUT_LAYER_COUNT);
    node_counts.push_back(HIDDEN_LAYER_COUNT);
    node_counts.push_back(OUTPUT_LAYER_COUNT);
    Network net(node_counts);

    std::vector<Sample *> samples;
    std::vector<Label *> labels;
    std::vector<Sample *> test_samples;
    std::vector<Label *> test_labels;

    assert(0 == load_minst_image(train_image_path, samples));
    assert(0 == load_minst_label(train_label_path, labels));
    assert(0 == load_minst_image(test_image_path, test_samples));
    assert(0 == load_minst_label(test_label_path, test_labels));
    printf("load train data finished \n");

    net.train(rate, round, samples, labels);
    printf("training finished \n");
    double ratio = evaluate(&net, test_samples, test_labels);
    printf("precise ratio : %f\n", ratio);

    exit(0);
}

/* function implementation */

static void
print_one_image(uint8_t *pixel, int nrow, int ncol) {
    for (int i = 0; i < nrow; i++) {
        for (int j = 0; j < ncol; j++) {
            printf("%03d ", pixel[i * ncol + j]);
        }
        printf("\n");
    }
}

static Sample *
pixel_to_sample(uint8_t *pixel, int size) {
    Sample *sample = new Sample(size);
    for (int i = 0; i < size; i++) {
        (*sample)[i] = pixel[i] / 256.0;
    }
    return sample;
}

static Label *
to_label(uint8_t digital) {
    Label *label = new Label(10, 0.1);
    (*label)[digital] = 0.9;
    return label;
}

int 
load_minst_image(const char *path, std::vector<Sample *> &samples) {
    uint32_t magic, num;
    uint32_t nrow, ncol;

    FILE *fp = fopen(path, "r");
    if (NULL == fp) {
        fprintf(stderr, "%s to open ", path);
        perror(" failed:");
        return -1;
    }
    fread(&magic, sizeof(magic), 1, fp);
    fread(&num, sizeof(num), 1, fp);
    fread(&nrow, sizeof(nrow), 1, fp);
    fread(&ncol, sizeof(ncol), 1, fp);
    magic = htonl(magic);
    num   = htonl(num);
    nrow  = htonl(nrow);
    ncol  = htonl(ncol);
    printf("magic:%u number:%u\n row:%u column:%u \n", magic, num, nrow, ncol);
    assert(magic == MAGIC_IMAGE);

    samples.clear();
    samples.resize(num, NULL);
    uint8_t *pixel = (uint8_t *)malloc(nrow * ncol);
    for (uint32_t i = 0; i < num; i++) {
        fread(pixel, sizeof(uint8_t), nrow * ncol, fp);
        Sample *p = pixel_to_sample(pixel, nrow * ncol);
        assert(NULL != p);
        samples[i] = p;
    }
    free(pixel);
    fclose(fp);
    return 0;
}

int 
load_minst_label(const char *path, std::vector<Label *> &labels) {
    uint32_t magic, num;
    uint8_t  value;

    FILE *fp = fopen(path, "r");
    if (NULL == fp) {
        fprintf(stderr, "%s to open ", path);
        perror(" failed:");
        return -1;
    }
    fread(&magic, sizeof(magic), 1, fp);
    fread(&num, sizeof(num), 1, fp);
    magic = htonl(magic);
    num   = htonl(num);
    printf("magic:%u number:%u\n", magic, num);
    assert(magic == MAGIC_LABEL);

    labels.clear();
    labels.resize(num, NULL);
    for (uint32_t i = 0; i < num; i++) {
        fread(&value, sizeof(value), 1, fp);
        Label *p = to_label(value);
        assert(NULL != p);
        labels[i] = p;
    }
    fclose(fp);
    return 0;
}

static int 
get_result(const Label &label) {
    int    index = 0;
    double max   = 0.0; 
    for (size_t i = 0; i < label.size(); i++) {
        if (label[i] > max) {
            max   = label[i];
            index = i;
        }
    }
    return index;
}

double evaluate(Network *net, 
        const std::vector<Sample *> &test_samples,
        const std::vector<Label *> &test_labels) {
    size_t count = 0;
    double ratio = 0.0; 
    std::vector<double> output;
    size_t size = test_samples.size();
    for (size_t i = 0; i < size; i++) {
        net->predict(*(test_samples[i]));

        output.clear();
        net->get_output(output);

        int eval_digit = get_result(output);
        int test_digit = get_result((*test_labels[i]));
        if (eval_digit == test_digit) {
            count += 1;
        }
    }
    ratio = count * 1.0 / size;
    return ratio;
}
