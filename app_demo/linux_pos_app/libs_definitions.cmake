# # 递归搜索匹配各内核源码目录下的.c文件
# FILE(GLOB KERNEL_SRCS ${KERNEL_SRC_DIR}/kernel/src/*.c)
# FILE(GLOB EMV_SRCS ${KERNEL_SRC_DIR}/emv/src/*.c)
# FILE(GLOB ENTRYPOINT_SRCS ${KERNEL_SRC_DIR}/entrypoint/src/*.c)
# FILE(GLOB AMEX_SRCS ${KERNEL_SRC_DIR}/amex/src/*.c)
# FILE(GLOB BANCOMAT_SRCS ${KERNEL_SRC_DIR}/bancomat/src/*.c)
# FILE(GLOB DISCOVER_SRCS ${KERNEL_SRC_DIR}/discover/src/*.c)
# FILE(GLOB JCB_SRCS ${KERNEL_SRC_DIR}/jcb/src/*.c)
# FILE(GLOB MIR_SRCS ${KERNEL_SRC_DIR}/mir/src/*.c)
# FILE(GLOB PAYPASS_SRCS ${KERNEL_SRC_DIR}/paypass/src/*.c)
# FILE(GLOB PAYWAVE_SRCS ${KERNEL_SRC_DIR}/paywave/src/*.c)
# FILE(GLOB PURE_SRCS ${KERNEL_SRC_DIR}/pure/src/*.c)
# FILE(GLOB QUICS_SRCS ${KERNEL_SRC_DIR}/quics/src/*.c)
# FILE(GLOB RUPAY_SRCS ${KERNEL_SRC_DIR}/rupay/src/*.c)

# # kernel module definition
# add_library(kernel SHARED ${KERNEL_SRCS})

# # emv module definition
# add_library(emv SHARED ${EMV_SRCS})

# # entrypoint module definition
# add_library(entrypoint SHARED ${ENTRYPOINT_SRCS})

# # amex module definition
# add_library(amex SHARED ${AMEX_SRCS})

# # bancomat module definition
# add_library(bancomat SHARED ${BANCOMAT_SRCS})

# # dpas module definition
# add_library(dpas SHARED ${DISCOVER_SRCS})

# # jcb module definition
# add_library(jcb SHARED ${JCB_SRCS})

# # mir module definition
# add_library(mir SHARED ${MIR_SRCS})

# # paypass module definition
# add_library(paypass SHARED ${PAYPASS_SRCS})

# # paywave module definition
# add_library(paywave SHARED ${PAYWAVE_SRCS})

# # pure module definition
# add_library(pure SHARED ${PURE_SRCS})

# # quics module definition
# add_library(quics SHARED ${QUICS_SRCS})

# # rupay module definition
# add_library(rupay SHARED ${RUPAY_SRCS})



set(LIBS kernelins)

