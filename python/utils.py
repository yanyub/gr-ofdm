from gnuradio import gr
from gruel import pmt

def vectors_to_packets(data, tags, lengthtagname):
    lengthtags = [t for t in tags
                  if pmt.pmt_symbol_to_string(t.key) == lengthtagname]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset."
                .format(lengthtagname))
        lengths[tag.offset] = pmt.pmt_to_long(tag.value)
    if 0 not in lengths:
        raise ValueError("There is no tag with key {0} and an offset of 0"
                         .format(lengthtagname))
    pos = 0
    packets = []
    while pos < len(data):
        if pos not in lengths:
            raise ValueError("There is no tag with key {0} and an offset of {1}."
                             "We were expecting one."
                             .format(lengthtagname, pos))
        length = lengths[pos]
        if length == 0:
            raise ValueError("Packets cannot have zero length.")
        if pos+length > len(data):
            raise ValueError("The final packet is incomplete.")
        packets.append(data[pos: pos+length])
        pos += length
    return packets

def packets_to_vectors(packets, lengthtagname):
    tags = []
    data = []
    offset = 0
    for packet in packets:
        data.extend(packet)
        tag = gr.gr_tag_t()
        tag.offset = offset
        tag.key = pmt.pmt_string_to_symbol(lengthtagname)
        tag.value = pmt.pmt_from_long(len(packet))
        tags.append(tag)
        offset = offset + len(packet)
    return data, tags
