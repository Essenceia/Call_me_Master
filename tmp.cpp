// flips discs in one direction
uint64_t flip_dir(const uint64_t P, const uint64_t O, const uint8_t move, const int dX, const int dY)
{
    uint64_t flips = 0;
    int i = (move % 8) + dX; // Starting index in x direction
    int j = (move / 8) + dY; // Starting index in y direction

    while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
    {
        const uint64_t bit = 1ULL << (j * 8 + i); // The bit to look at
        if (O & bit) // The bit belongs to the opponent
            flips |= bit; // Add to possible flips
        else if (P & bit) // The bit belongs to the player
            return flips; // All possible flips become real flips
        else // The bit belongs to no player
            return 0; // There are no possible flips
        i += dX; // Advance in direction
        j += dY; // Advance in direction
    }
    return 0;
}

uint64_t flip(const uint64_t P, const uint64_t O, const uint8_t move)
{
    return move == 64 ? 0ULL :
           flip_dir(P, O, move, -1, -1)
           | flip_dir(P, O, move, -1,  0)
           | flip_dir(P, O, move, -1, +1)
           | flip_dir(P, O, move,  0, -1)
           | flip_dir(P, O, move,  0, +1)
           | flip_dir(P, O, move, +1, -1)
           | flip_dir(P, O, move, +1,  0)
           | flip_dir(P, O, move, +1, +1);
}
while (!GAME_OVER && has_all_clients()) {
if (message_share[GC].active) {
Contro->tosend_msg = message_share[GC].tosend;
//we want to share data to the client
if (sendof(Contro->dest_socket, Contro->tosend_msg) == 0) {
printf("INFO_%d:Message sucessfully sent to client \n", getpid());
} else printf("ERRO_%d:Message failed sent to client \n", getpid());
message_share[GC].active = 0;

}
//check if we recived new msg
Contro->rev_msg = recive_msg(Contro->dest_socket);
if (Contro->rev_msg != NULL) {
//todo - check for recived messages
if (Contro->rev_msg->type == SKIP_TURN || Contro->rev_msg->type == OK_NOK) {
//forward send message to player
player = (CLIENT_LISR) Contro->rev_msg->msg[0];
message_share[player].tosend = recapsulate_for_player(Contro->rev_msg);
message_share[player].active = 1;
printf("INFO_Client: Forwarding message to player handler with id %x msg : %s \n",
player, (char *) message_share[player].tosend->msg);


} else {
printf("ERRO_%d: Unexpected message recived form controller message is of type %x", getpid(),
        Contro->rev_msg->type);
}
destroy_msg(Contro->rev_msg);

}
}