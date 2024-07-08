file(GLOB BUSINESS_PRIVATE_SRCS ${KERNELINS_SRC_DIR}/business/*.c)
file(GLOB PROJECT_SRCS ${KERNELINS_SRC_DIR}/src/*.c)
file(GLOB EMV_KERNEL_INS_SRCS ./src/emv_kernel_ins/*.c)
file(GLOB THIRD_PARTY_JSON_SRCS ${KERNELINS_SRC_DIR}/third_party/cJSON_1.7.15/*.c)
file(GLOB THIRD_PARTY_TMS_DSP_SRCS ${KERNELINS_SRC_DIR}/third_party/internal_libarys/tms_dspread/*.c)


set(kernelins_SOURCES ${BUSINESS_PRIVATE_SRCS}  ${PROJECT_SRCS} ${EMV_KERNEL_INS_SRCS}  ${THIRD_PARTY_JSON_SRCS}  ${THIRD_PARTY_TMS_DSP_SRCS})