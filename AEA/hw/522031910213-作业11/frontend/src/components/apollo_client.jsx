import { ApolloClient, InMemoryCache, HttpLink } from '@apollo/client';
import { BASEURL } from '../services/requestService';

const client = new ApolloClient({
  link: new HttpLink({
    uri: `${BASEURL}/graphql`,
  }),
  cache: new InMemoryCache(),
});

export default client;
