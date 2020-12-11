#ifndef __AVFORMAT_H__
#define __AVFORMAT_H__

#include "audio_common.h"

#define AVIOContext FILE

/**
 * This structure contains the data a format has to probe a file.
 */

typedef struct AVProbeData {
	const char *filename;
	/*< Buffer must have AVPROBE_PADDING_SIZE of
	extra allocated bytes filled with zero. */
	unsigned char	*buf;
	int buf_size;       /* < Size of buf except extra allocated bytes */
	const char *mime_type; /* < mime_type, when known. */
} AVProbeData;

#define AVPROBE_SCORE_MAX       100 /* < maximum score */

typedef struct AVIndexEntry {
	int64_t pos;
	/**<
	 * Timestamp in AVStream.time_base units, preferably the time from which
	 * on correctly decoded frames are available
	 * when seeking to this entry. That means preferable
	 * PTS on keyframe based formats.
	 * But demuxers can choose to store a different timestamp, if it is more
	 * convenient for the implementation or nothing better
	 * is known
	 */
	int64_t timestamp;
#define AVINDEX_KEYFRAME 0x0001
	/**
	* Flag is used to indicate which frame should be discarded after decoding.
	*/
#define AVINDEX_DISCARD_FRAME  0x0002
	int flags:2;
	/* Yeah, trying to keep the size of this small to reduce memory
	requirements (it is 24 vs. 32 bytes due to possible 8-byte alignment).*/
	int size:30;
	/**< Minimum distance between this and the previous keyframe,
	used to avoid unneeded searching. */
	int min_distance;
} AVIndexEntry;

#define AV_DISPOSITION_DEFAULT   0x0001

/**
 * Stream structure.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVStream) must not be used outside libav*.
 */
typedef struct AVStream {
	int index;    /**< stream index in AVFormatContext */
	/**
	 * Format-specific stream ID.
	 * decoding: set by libavformat
	 * encoding: set by the user, replaced by libavformat if left unset
	 */
	int id;
	void *priv_data;
	/**
	 * This is the fundamental unit of time (in seconds) in terms
	 * of which frame timestamps are represented.
	 *
	 * decoding: set by libavformat
	 * encoding: May be set by the caller before avformat_write_header() to
	 *           provide a hint to the muxer about the desired timebase. In
	 *           avformat_write_header(), the muxer will overwrite this field
	 *           with the timebase that will actually be used for the timestamps
	 *           written into the file (which may or may not be related to the
	 *           user-provided one, depending on the format).
	 */
	AV_Rational time_base;

	/**
	 * Decoding: pts of the first frame of the stream in presentation order, in stream time base.
	 * Only set this if you are absolutely 100% sure that the value you set
	 * it to really is the pts of the first frame.
	 * This may be undefined (AV_NOPTS_VALUE).
	 * @note The ASF header does NOT contain a correct start_time the ASF
	 * demuxer must NOT set this.
	 */
	/*    int64_t start_time;*/

	/**
	 * Decoding: duration of the stream, in stream time base.
	 * If a source file does not specify a duration, but does specify
	 * a bitrate, this value will be estimated from bitrate and file size.
	 */
	int64_t duration;

	int64_t nb_frames; /* < number of frames in this stream if known or 0 */

	int disposition; /**< AV_DISPOSITION_* bit field */
	int64_t cur_dts;
	AVIndexEntry
	*index_entries; /**< Only used if the format does not support seeking natively. */
	int nb_index_entries;
	unsigned int index_entries_allocated_size;
	/**
	 * Number of samples to skip at the start of the frame decoded from the next packet.
	 */
	int skip_samples;
	/*
	 * Codec parameters associated with this stream. Allocated and freed by
	 * libavformat in avformat_new_stream() and avformat_free_context()
	 * respectively.
	 *
	 * - demuxing: filled by libavformat on stream creation or in
	 *             avformat_find_stream_info()
	 * - muxing: filled by the caller before avformat_write_header()
	 */
	AV_CodecParameters *codecpar;
} AVStream;

/**
 * Format I/O context.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVFormatContext) must not be used outside libav*, use
 * avformat_alloc_context() to create an AVFormatContext.
 *
 * Fields can be accessed through AVOptions (av_opt*),
 * the name string used matches the associated command line parameter name and
 * can be found in libavformat/options_table.h.
 * The AVOption/command line parameter names differ in some cases from the C
 * structure field names for historic reasons or brevity.
 */
typedef struct AVFormatContext {
	/**
	 * Format private data. This is an AVOptions-enabled struct
	 * if and only if iformat/oformat.priv_class is not NULL.
	 *
	 * - muxing: set by avformat_write_header()
	 * - demuxing: set by avformat_open_input()
	 */
	void *priv_data;

	/**
	 * I/O context.
	 *
	 * - demuxing: either set by the user before avformat_open_input() (then
	 *             the user must close it manually) or set by avformat_open_input().
	 * - muxing: set by the user before avformat_write_header(). The caller must
	 *           take care of closing / freeing the IO context.
	 *
	 * Do NOT set this field if AVFMT_NOFILE flag is set in
	 * iformat/oformat.flags. In such a case, the (de)muxer will handle
	 * I/O in some other way and this field will be NULL.
	 */
	AVIOContext *pb;


	/* stream info */
	/**
	 * Flags signalling stream properties. A combination of AVFMTCTX_*.
	 * Set by libavformat.
	 */
	/*    int ctx_flags;*/

	/**
	 * Number of elements in AVFormatContext.streams.
	 *
	 * Set by avformat_new_stream(), must not be modified by any other code.
	 */
	unsigned int nb_streams;
	/**
	 * A list of all streams in the file. New streams are created with
	 * avformat_new_stream().
	 *
	 * - demuxing: streams are created by libavformat in avformat_open_input().
	 *             If AVFMTCTX_NOHEADER is set in ctx_flags, then new streams may also
	 *             appear in av_read_frame().
	 * - muxing: streams are created by the user before avformat_write_header().
	 *
	 * Freed by libavformat in avformat_free_context().
	 */
	AVStream * streams[16];

	/**
	 * Flags modifying the (de)muxer behaviour. A combination of AVFMT_FLAG_*.
	 * Set by the user before avformat_open_input() / avformat_write_header().
	 */
	int flags;

	/**
	 * Number of chapters in AVChapter array.
	 * When muxing, chapters are normally written in the file header,
	 * so nb_chapters should normally be initialized before write_header
	 * is called. Some muxers (e.g. mov and mkv) can also write chapters
	 * in the trailer.  To write chapters in the trailer, nb_chapters
	 * must be zero when write_header is called and non-zero when
	 * write_trailer is called.
	 * - muxing: set by user
	 * - demuxing: set by libavformat
	 */
	unsigned int nb_chapters;
	/**
	 * The maximum number of streams.
	 * - encoding: unused
	 * - decoding: set by user through AVOptions (NO direct access)
	 */
	int max_streams;
} AVFormatContext;

#define AVSEEK_FLAG_BACKWARD 1 /* < seek backward */
#define AVSEEK_FLAG_BYTE     2 /* < seeking based on position in bytes */
#define AVSEEK_FLAG_ANY      4 /* < seek to any frame, even non-keyframes */
#define AVSEEK_FLAG_FRAME    8 /* < seeking based on frame number */

#endif /* __AVFORMAT_H__ */
