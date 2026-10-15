# Makefile of the library "external/opusfile"
# David Weenink, 23 May 2026

CPPFLAGS += -I ../../melder -I ../vorbis -I opus -I opus/celt -I opus/silk -I opus/silk/float

OPUSFILE_OBJECTS = opusfile.o opusfile_info.o opusfile_internal.o opusfile_stream.o

OPUS_OBJECTS = opus/analysis.o opus/mlp.o opus/opus.o opus/opus_decoder.o opus/opus_multistream.o \
opus/mapping_matrix.o opus/mlp_data.o opus/opus_multistream_decoder.o \
opus/opus_projection_decoder.o opus/repacketizer.o

CELT = opus/celt/

CELT_OBJECTS = $(CELT)bands.o $(CELT)celt.o $(CELT)celt_encoder.o $(CELT)celt_decoder.o \
$(CELT)cwrs.o $(CELT)entcode.o $(CELT)entdec.o $(CELT)entenc.o $(CELT)kiss_fft.o \
$(CELT)laplace.o $(CELT)mathops.o $(CELT)mdct.o $(CELT)modes.o $(CELT)pitch.o \
$(CELT)celt_lpc.o $(CELT)quant_bands.o $(CELT)rate.o $(CELT)vq.o

SILK = opus/silk/

SILK_OBJECTS = $(SILK)CNG.o $(SILK)code_signs.o $(SILK)init_decoder.o $(SILK)decode_core.o $(SILK)decode_frame.o \
$(SILK)decode_parameters.o $(SILK)decode_indices.o $(SILK)decode_pulses.o $(SILK)decoder_set_fs.o $(SILK)dec_API.o \
$(SILK)enc_API.o $(SILK)encode_indices.o $(SILK)encode_pulses.o $(SILK)gain_quant.o $(SILK)interpolate.o \
$(SILK)LP_variable_cutoff.o $(SILK)NLSF_decode.o $(SILK)NSQ.o $(SILK)NSQ_del_dec.o $(SILK)PLC.o \
$(SILK)shell_coder.o $(SILK)tables_gain.o $(SILK)tables_LTP.o $(SILK)tables_NLSF_CB_NB_MB.o $(SILK)tables_NLSF_CB_WB.o \
$(SILK)tables_other.o $(SILK)tables_pitch_lag.o $(SILK)tables_pulses_per_block.o \
$(SILK)VAD.o $(SILK)control_audio_bandwidth.o $(SILK)quant_LTP_gains.o $(SILK)VQ_WMat_EC.o \
$(SILK)HP_variable_cutoff.o $(SILK)NLSF_encode.o $(SILK)NLSF_VQ.o $(SILK)NLSF_unpack.o $(SILK)NLSF_del_dec_quant.o \
$(SILK)process_NLSFs.o $(SILK)stereo_LR_to_MS.o $(SILK)stereo_MS_to_LR.o $(SILK)check_control_input.o \
$(SILK)control_SNR.o $(SILK)init_encoder.o $(SILK)control_codec.o $(SILK)A2NLSF.o $(SILK)ana_filt_bank_1.o \
$(SILK)biquad_alt.o $(SILK)bwexpander_32.o $(SILK)bwexpander.o $(SILK)debug.o $(SILK)decode_pitch.o \
$(SILK)inner_prod_aligned.o $(SILK)lin2log.o $(SILK)log2lin.o $(SILK)LPC_analysis_filter.o $(SILK)LPC_inv_pred_gain.o \
$(SILK)table_LSF_cos.o $(SILK)NLSF2A.o $(SILK)NLSF_stabilize.o $(SILK)NLSF_VQ_weights_laroia.o $(SILK)pitch_est_tables.o \
$(SILK)resampler.o $(SILK)resampler_down2_3.o $(SILK)resampler_down2.o $(SILK)resampler_private_AR2.o \
$(SILK)resampler_private_down_FIR.o $(SILK)resampler_private_IIR_FIR.o $(SILK)resampler_private_up2_HQ.o \
$(SILK)resampler_rom.o $(SILK)sigm_Q15.o $(SILK)sort.o $(SILK)sum_sqr_shift.o $(SILK)stereo_decode_pred.o \
$(SILK)stereo_encode_pred.o $(SILK)stereo_find_predictor.o $(SILK)stereo_quant_pred.o $(SILK)LPC_fit.o

SILKFLOAT = opus/silk/float/

SILKFLOAT_OBJECTS = $(SILKFLOAT)apply_sine_window_FLP.o $(SILKFLOAT)autocorrelation_FLP.o \
$(SILKFLOAT)burg_modified_FLP.o $(SILKFLOAT)bwexpander_FLP.o $(SILKFLOAT)corrMatrix_FLP.o \
$(SILKFLOAT)encode_frame_FLP.o $(SILKFLOAT)energy_FLP.o $(SILKFLOAT)find_LPC_FLP.o $(SILKFLOAT)find_LTP_FLP.o \
$(SILKFLOAT)find_pitch_lags_FLP.o $(SILKFLOAT)find_pred_coefs_FLP.o $(SILKFLOAT)inner_product_FLP.o $(SILKFLOAT)k2a_FLP.o \
$(SILKFLOAT)LPC_analysis_filter_FLP.o $(SILKFLOAT)LPC_inv_pred_gain_FLP.o $(SILKFLOAT)LTP_analysis_filter_FLP.o \
$(SILKFLOAT)LTP_scale_ctrl_FLP.o $(SILKFLOAT)noise_shape_analysis_FLP.o $(SILKFLOAT)pitch_analysis_core_FLP.o \
$(SILKFLOAT)process_gains_FLP.o $(SILKFLOAT)regularize_correlations_FLP.o $(SILKFLOAT)residual_energy_FLP.o \
$(SILKFLOAT)scale_copy_vector_FLP.o $(SILKFLOAT)scale_vector_FLP.o $(SILKFLOAT)schur_FLP.o $(SILKFLOAT)sort_FLP.o \
$(SILKFLOAT)warped_autocorrelation_FLP.o $(SILKFLOAT)wrappers_FLP.o

OBJECTS = $(OPUSFILE_OBJECTS) \
	$(OPUS_OBJECTS) \
	$(CELT_OBJECTS) \
	$(SILK_OBJECTS) \
	$(SILKFLOAT_OBJECTS)

.PHONY: all clean

all: libopusfile.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libopusfile.a

libopusfile.a: $(OBJECTS)
	ar rus libopusfile.a $(OBJECTS)

$(OBJECTS): *.h ../../melder/*.h
